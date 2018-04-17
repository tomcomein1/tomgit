EXEC SQL include "para_macro.h";
EXEC SQL include "dbtools.h";
EXEC SQL include "ifx_face.h";

mint has_trans=1;

/*连接数据库*/
int connect_database(char *db_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char dbname[MAX_DB_LEN];
	EXEC SQL END DECLARE SECTION;

	memset(dbname, 0x00, sizeof(dbname) );
	strcpy (dbname, db_name) ;

EXEC SQL connect to :
	dbname ;
	if (sqlca.sqlcode != 0)
	{
		SHOWSQLERR;
                strcpy (ErrInfo, "连接数据库错");
	}

	if (_DEBUG)
		printf ("Connect database [%s] successful!\n", dbname);

	return sqlca.sqlcode;
}

/*关闭数据库*/
int close_database()
{
	EXEC SQL disconnect current;
	if (sqlca.sqlcode)
        {
	    SHOWSQLERR;
	    strcpy (ErrInfo, "关闭数据库错");
        }

	return sqlca.sqlcode;
}

/*运行一个非查询类sql语句*/
int runsql(char *_sql)
{
	EXEC SQL begin declare section;
	char sqlstr[MAX_LINE_LEN];
	EXEC SQL end   declare section;
        int ret = 0;

	memset(sqlstr,0x00,sizeof(sqlstr));
	strcpy(sqlstr, _sql);

        if(has_trans)  EXEC SQL begin work;
EXEC SQL EXECUTE IMMEDIATE :
	sqlstr;
	if (sqlca.sqlcode)
	{
		SHOWSQLERR;
                ret = sqlca.sqlcode;
                if (has_trans) EXEC SQL rollback work;
	        strcpy (ErrInfo, "执行sql失败");
		return ret;
	}

	if ( has_trans ) EXEC SQL commit work;

	return sqlca.sqlcode;
}

/*执行sql语句块*/
int exec_sql(char *sql, char *file, char *retbuff)
{
	EXEC SQL begin declare section;
	char sqlstr[MAX_LINE_LEN];
	EXEC SQL end   declare section;

        char chksql[10];

        memset(chksql, 0x00, sizeof(chksql) );
        strncpy(chksql, sql, 6);
        lower(chksql);

        strcpy(sqlstr, sql);

#if 0
	if (sqlstr[0] == NULL
	    && !strstr(chksql, "update")
	    && !strstr(chksql, "delete")
	    && !strstr(chksql, "create")
	    && !strstr(chksql, "insert")
	    && !strstr(chksql, "select")
	    && !strstr(chksql, "drop") )
		return -1;
#endif

	/* EXEC SQL WHENEVER ERROR STOP; */
	EXEC SQL set lock mode to wait 30;

	WriteLog (ERR_LVL, "Execute: %s", sqlstr);

	if ( !strcmp(chksql, "select") ) return sql_select(sqlstr, file, retbuff);

	return runsql(sqlstr);
}

int sql_select (slctstmt, fname, outbuff)
EXEC SQL BEGIN DECLARE SECTION;
char *slctstmt;
char *fname;
char *outbuff;
EXEC SQL END DECLARE SECTION;
{
	register mint pos;
	register char *cp;
	register mint len;
	register mint i;
	char field[FLDSIZE+1];
	char *fieldp;
	struct sqlda *udesc;
	struct sqlvar_struct *col;
	FILE *unlfile; 
	char delim = '|'; 
        char delims[4] = "$|$";
	char *buffer = NULL;
	int rows = 0;
        int ret = 0;
        int file_flag = 0;

        /* if(has_trans)  EXEC SQL begin work; */

EXEC SQL prepare usqlobj from :
	slctstmt;
	if ( sqlca.sqlcode != 0 )
	{
		WriteLog(ERR_LVL, "prepare sql error! [%d]", sqlca.sqlcode);
	        strcpy (ErrInfo, "prepare sql error");
		return sqlca.sqlcode;
	}
	EXEC SQL describe usqlobj into udesc;
	if ( sqlca.sqlcode != 0 )
	{
		WriteLog(ERR_LVL, "describe sql error! [%d]", sqlca.sqlcode);
	        strcpy (ErrInfo, "describe sql error");
		return sqlca.sqlcode;
	}

        if (strlen(fname) > 0) file_flag = 1;

        if (file_flag)
        {
    	    unlfile = fopen(fname, "w");
            if ( unlfile == NULL )
	    {
	         WriteLog(ERR_LVL,"Cannot open file '%s'\n", fname);
	         strcpy (ErrInfo, "Open File error");
	         return -1;
	    }
        }

	/* Step 1: analyze udesc to determine type and memory requirements
      *         of each item in the select list.  rtypalign() returns a
      *         pointer to the next appropriate boundary (starting at
      *         pos) for the indicated data type.
      */
	/* WriteLog(ERR_LVL, "**********************************\n"); */
	pos = 0;
	for (col = udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	{
		/* printf( "%s ", col->sqlname); */
		switch(col->sqltype)
		{
		case SQLSMFLOAT:
			col->sqltype = CFLOATTYPE;
			break;

		case SQLFLOAT:
			col->sqltype = CDOUBLETYPE;
			break;

		case SQLMONEY:
		case SQLDECIMAL:
			col->sqltype = CDECIMALTYPE;
			break;

		case SQLCHAR:
			col->sqltype = CCHARTYPE;
			break;

		case SQLINT:
		case SQLSERIAL:
			col->sqltype = CINTTYPE;
			break;

		case SQLDATE:
			col->sqltype = CDATETYPE;
			break;

		case SQLDTIME:
			col->sqltype = CDTIMETYPE;
			break;

		case SQLVCHAR:
			col->sqltype = CVCHARTYPE;
			break;

		default:
			/*  The program does not handle INTEGER,
		     *  SMALL INTEGER, DATE, SERIAL or other
		     *  data types.  Do nothing if we see
		     *  an unsupported type. 
		     */
			WriteLog (ERR_LVL, "Don't support this type: %d", col->sqltype); 
	                strcpy (ErrInfo, "Don't support this type");
			return -1;
		}
		col->sqllen = rtypmsize(col->sqltype,col->sqllen);
		pos = rtypalign( pos, col->sqltype) + col->sqllen;
		col->sqlind = NULL;
	}
	/* WriteLog(ERR_LVL, "\n" ); */

	/* Step 2: Allocate memory to receive a row of the table returned
      *         by the SELECT statement.  The variable pos has an integer
      *         value equal to the number of bytes occupied by the row.
      */
	buffer = malloc(pos);
	if ( buffer == NULL )
	{
		WriteLog(ERR_LVL, "Out of memory");
		strcpy (ErrInfo, "Out of memory");
		return -1;
	}

	/* Step 3: Set pointers in the allocated memory to receive each
      *         item in the select list.
      */
	cp = buffer;
	for (col = udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	{
		cp = (char *) rtypalign( (mlong) cp, col->sqltype);
		col->sqldata = cp;
		cp += col->sqllen;
	}

	/* Step 4: Fetch each row of the query, convert to ASCII format, and
      *         write to the output file.
      */
	EXEC SQL declare usqlcurs cursor for usqlobj;

	/* if ( has_trans ) EXEC SQL begin work; */

	EXEC SQL open usqlcurs;
	EXEC SQL fetch usqlcurs using descriptor udesc;
	/* WriteLog(ERR_LVL, "--------------------------------------\n"); */
	while (sqlca.sqlcode == 0 )
	{
		for (col=udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
		{
			byfill(field,FLDSIZE+1,0);
			fieldp = field;
			switch (col->sqltype)
			{

			case CFLOATTYPE:
				sprintf(field,"%f",
				    (double) *((float *) (col->sqldata)));
				break;

			case CDOUBLETYPE:
				sprintf(field,"%f",*((double *) (col->sqldata)));
				break;

			case CDECIMALTYPE:
				dectoasc( (dec_t *) col->sqldata, field, FLDSIZE,-1);
				break;

			case CCHARTYPE:
				fieldp = col->sqldata;
				break;

			case CINTTYPE:
				sprintf(field, "%d", *((int *) (col->sqldata)));
				break;

			case CDATETYPE:
				rfmtdate(*((int *)(col->sqldata)), "mm/dd/yyyy", fieldp);
				break;

			case CDTIMETYPE:
				fieldp =  col->sqldata;
				break;

			case CVCHARTYPE:
				fieldp = col->sqldata;
				break;

			default:
				/*  Usupported data type.  */
				break;

			}
			len = stleng(fieldp);
			cp = fieldp + len - 1;
			while ( len > 1 && *cp == ' ' )
			{
				*cp = '\0';
				len--, cp--;
			}
                        if (file_flag)
                        {
			    fputs(fieldp, unlfile); 
	                    putc(delim, unlfile); 
                        }

                        if (!file_flag)
                        {
                            if ((strlen(outbuff)+strlen(fieldp)+strlen(delims) ) > (MAX_BUFF_LEN - 20) )
                                break;
			    sprintf( outbuff+strlen(outbuff), "%s%s", fieldp, delims);
                        }
		}

		if (file_flag) putc('\n', unlfile);

		if (!file_flag)
                {
		    if ( (strlen(outbuff)+ strlen(delims) + 2 ) > (MAX_BUFF_LEN - 20) )
                        break;
	            strcat(outbuff, "\r\n"); 
                }

		rows++;
		/* if ((rows % 50) == 0) getchar(); */

		EXEC SQL fetch usqlcurs using descriptor udesc;
	}

        if (file_flag) fclose(unlfile); 
	free(buffer);
	if ( sqlca.sqlcode && sqlca.sqlcode != SQLNOTFOUND )
	{
		/* display ERROR message	*/
		WriteLog(ERR_LVL, "select failed sqlcode = %d", sqlca.sqlcode);
		strcpy (ErrInfo, "select failed");
                ret=sqlca.sqlcode;
	}
	else 
	{
		WriteLog(ERR_LVL, "%d rows select" , sqlca.sqlerrd[2]);
	}
	/* WriteLog(ERR_LVL, "**********************************\n"); */

	EXEC SQL close usqlcurs;
	/* if ( has_trans ) EXEC SQL commit work;  */
	free(udesc);

	return ret;
}


/** 恢复数据有内容 **/
int para_restore(char *fname, char *tabname)
{
EXEC SQL BEGIN DECLARE SECTION;
    struct ifx_table_cols  ifx_tab[128];
    char sqlstr[MAX_SQL_LEN];
EXEC SQL END DECLARE SECTION;

    FILE *fp = NULL;
    int col_id = 0;
    char line[MAX_LINE_LEN];
    char value[500];
    int i = 0;
    int no = 0;
    int ret = 0;

    fp = fopen (fname, "r");
    if (fp == NULL)
    {
	WriteLog(ERR_LVL,"Cannot open file '%s'\n", fname);
	strcpy (ErrInfo, "Open File error");
	return -1;
    }

    memset(sqlstr, 0, sizeof(sqlstr));
    has_trans = 0;

    EXEC SQL begin work;
    sprintf(sqlstr, "delete from %s where 1=1", tabname);

    WriteLog(ERR_LVL,"exec sql:[%s]", sqlstr);
    ret = db_ifx_nquery(sqlstr);
    if (ret != 0)
    {
	SHOWSQLERR;
	strcpy (ErrInfo, "执行恢复删除数据表出错");
        return ret;
    }

    memset(sqlstr, 0, sizeof(sqlstr));
    sprintf(sqlstr, SQL_STMT, tabname);
    WriteLog(ERR_LVL,"sqlstr:[%s]", sqlstr);

    EXEC SQL prepare sql_stmt_pre from :sqlstr;
    if(sqlca.sqlcode != 0)
    {
	SHOWSQLERR;
	strcpy (ErrInfo, "prepare error");
        return sqlca.sqlcode;
    }
    EXEC SQL declare sql_stmt_cur cursor for sql_stmt_pre;
    if(sqlca.sqlcode != 0)
    {
	strcpy (ErrInfo, "declare cursor error");
        ret = sqlca.sqlcode;
        goto ERROR;
    }
    
    EXEC SQL open sql_stmt_cur;
    if (sqlca.sqlcode)
    {
	strcpy (ErrInfo, "open cursor error");
        ret = sqlca.sqlcode;
        goto ERROR;
    }

    while(1)
    {
        memset(&ifx_tab[col_id], 0x00, sizeof(struct ifx_table_cols) );
        EXEC SQL fetch sql_stmt_cur into :ifx_tab[col_id];
        if (sqlca.sqlcode != 0 && sqlca.sqlcode != 100)
        {
	    strcpy (ErrInfo, "fetch cursor error");
            ret = sqlca.sqlcode;
            goto ERROR;
        }
        else if (sqlca.sqlcode==100)
        {
            ret = 0;
            break;
        }
        rtrim(ifx_tab[col_id].colname);
        /* WriteLog(ERR_LVL,"col_id[%d] colname[%s]coltype[%ld]", col_id, ifx_tab[col_id].colname, ifx_tab[col_id].coltype ); */
        col_id++;
   }

   while(1)
   {
         memset(line, 0, sizeof(line) );
         memset(sqlstr, 0, sizeof(sqlstr));
         if (get_line(line, fp, '\n') <= 0)
            break;
         no++;
         sprintf(sqlstr, "insert into %s values(", tabname);
         for (i=0; i<col_id; i++)
         { 
             memset(value, 0, sizeof(value) );
             get_sub_str(line, value, '|', i+1); 
             switch(ifx_tab[col_id].coltype)
             {
               case SQLCHAR:
               case SQLVCHAR:
               case SQLTEXT:
                  sprintf(sqlstr + strlen(sqlstr), "'%s', ", value);
                  break;
                case SQLSMINT:
                case SQLINT:
                case SQLSERIAL:
                case SQLDATE:
                case SQLINT8:
                case 258:
                  sprintf(sqlstr + strlen(sqlstr), "%d, ", atol(value));
                  break;
                case SQLFLOAT:
                case SQLSMFLOAT:
                case SQLDECIMAL:
                case 261:
                  sprintf(sqlstr + strlen(sqlstr), "%f, ", atof(value));
                  break;
                default:
                  sprintf(sqlstr + strlen(sqlstr), "'%s', ", value);
                  break; 
             }
         }
         sqlstr[strlen(sqlstr)-2] = '\0';
         sprintf(sqlstr + strlen(sqlstr), ")");
         WriteLog(ERR_LVL,"[%d] sqlstr:[%s]", no, sqlstr);
         ret = db_ifx_nquery(sqlstr);
         if (ret != 0) 
         {
              ret = sqlca.sqlcode;
	      strcpy (ErrInfo, "执行insert出错");
              goto ERROR;    
         }
   }

ERROR:
    SHOWSQLERR;
    EXEC SQL free sql_stmt_pre;
    EXEC SQL close sql_stmt_cur;
    EXEC SQL free sql_stmt_cur;
    fclose(fp);

    if (ret != 0) EXEC SQL rollback work;
    else EXEC SQL commit work;

    return ret;
}

