/*一个简单的SQL命令行执行器*/
/*配置文件为sql语句 默认读取执行路径下exec_sql.ini*/
/*exec_sql.ec*/

#include <stdio.h>
#include <stdlib.h>
EXEC SQL include sqlca;
EXEC SQL include sqlda;
EXEC SQL include sqltypes;
EXEC SQL include decimal;
#define FLDSIZE 255

#define MAX_LINE_LEN 1024
mint has_trans;

static char *lower_str(char *str, int length);

void usage (char *prname)
{
	printf ("%s -d [database] -f [filename] -s [sql] -t [sqlno] -h \n", prname);
}

int main(int argc , char *argv[])
{
	char database[80] = "ipssdb";
	char filename[256] = "exec_sql.ini";
	char sqlstr[MAX_LINE_LEN] = "\0";
	char *p = NULL;
	char sqlno[10] = "\0";
	int ch, runsql = 0;
	int ret = 0;
	
	while ( (ch=getopt(argc, argv, "d:f:s:t:h")) != -1)
	{
		switch (ch)
		{
			case 'd':
				p = database;
				break;
			case 'f':
				p = filename;
				break;
		    case 's':
		        runsql = 1;
		        p = sqlstr;
		        break;
			case 'h':
				usage(argv[0]);
				return 0;
			case 't':
			    runsql = 2;
			    p = sqlno;
			    break;
			default : 
				usage(argv[0]); 
				return 0;
		}
		memcpy (p, optarg, strlen(optarg) );
		p[strlen(optarg)] = '\0';
	}
	/* printf ("%s -d %s -f %s -s %s -t %s\n", 
	            argv[0], database, filename, sqlstr, sqlno); */

    ret = open_database(database);
    if (ret < 0)
    {
        fprintf(stderr, "open database [%s] error![%d]\n", database, SQLCODE);
        return -1;
    }
    fprintf(stdout, "open database [%s] successful!\n", database);
    has_trans = ( sqlca.sqlwarn.sqlwarn1 == 'W' );
    
    switch(runsql)
    {
        case 1: /* -s */
            ret = exec_sql (sqlstr);
            break;
        case 2: /* -t */
            ret = get_table_sql(sqlno, sqlstr);
            if (ret == 0)
                ret = exec_sql(sqlstr);
            break;
        default:
            ret = exec_file (filename);
            break;
    }
    
    fprintf(stderr, "exec function returns : [%d]\n", ret);

    return close_database();
}

/*连结数据库*/
int open_database(char *db)
{
    EXEC SQL begin declare section;
        char dbname[80]; 
    EXEC SQL end   declare section;
    
    strcpy(dbname, db);
    EXEC SQL database :dbname;
    
    return sqlca.sqlcode;
}

int close_database()
{
    EXEC SQL close database;
    return sqlca.sqlcode;
}

/*执行sql语句块*/
int exec_sql(char *sql)
{
    EXEC SQL begin declare section;
        char sqlstr[MAX_LINE_LEN]; 
    EXEC SQL end   declare section;
    
    strcpy(sqlstr, sql);
    
    lower_str(sqlstr, 6);
    if (sqlstr[0] == NULL
        && !strstr(sqlstr, "update")
        && !strstr(sqlstr, "delete")
        && !strstr(sqlstr, "create")
        && !strstr(sqlstr, "drop") 
        && !strstr(sqlstr, "select")
        && !strstr(sqlstr, "alter") )
        return -1;

    /* EXEC SQL WHENEVER ERROR STOP; */
    EXEC SQL set lock mode to wait 30;

    fprintf(stdout, "Execute: %s \n", sqlstr);

    if (strstr(sqlstr, "select")) return sql_select(sqlstr);
    
    if ( has_trans )
         EXEC SQL begin work;

    EXEC SQL EXECUTE IMMEDIATE :sqlstr;
    
    fprintf(stdout, "Execute Sqlcode: [%d]\n", sqlca.sqlcode);
    if ( has_trans )
        EXEC SQL commit work;

    return sqlca.sqlcode;
}

/* tolower */
static char *lower_str(char *str, int length)
{
    char *p = str;

    while (length--)
        p[length] = tolower(str[length]);

    return p;
}

/*读一行，以分行或文件结束标志结束*/
int get_line(char * sline, FILE * fp)
{
    char ch=0;
    long nRec=0;

    while(ch != ';' && ch != EOF )
    {
        if (nRec >= MAX_LINE_LEN) break;
        ch=getc(fp);
        sline[nRec++]=ch;
    }

    if(nRec >= MAX_LINE_LEN) return -1;

    sline[nRec]='\0';
    if(ch == ';') return 1;
    if(ch == EOF) return 0;

    return -1;
}

/*执行配置文件*/
int exec_file(char *fn)
{
    FILE *fp = NULL;
    char line[MAX_LINE_LEN];
    int ret = 0;
    char *p = NULL;
    
    fprintf(stdout, "read file: [%s]\n", fn);
    if (NULL == (fp = fopen(fn, "r") ) )
    {
        fprintf(stderr, "open file [%s] faulse!\n", fn);
        return -1;
    }
    
    while (1)
    {
        memset (line, 0x00, sizeof(line));
        
        if (get_line(line, fp)<=0) break;
        
        p = line;
        while (*p == '\n')
            *p=' ', p++; /*将前回车替换成空格*/

        /*#，和-开头为注释*/
        if (*p != '#' && *p != '-')
            ret = exec_sql (p) ; 
        else
            fprintf(stdout, "Annotate: %s \n", p);
    }

    fclose(fp);
    return 0;
}

int get_table_sql(char *sqlno, char *sql_text)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char no[10];
        char text_str[1024];
        int len = 0;
    EXEC SQL END DECLARE SECTION;
    
    strcpy(no, sqlno);
    
    memset (text_str, 0x00, MAX_LINE_LEN);
    EXEC SQL select rtrim(sql_text) into :text_str 
               from exec_sql_tab
              where sqlno = :no ;

    len = strlen(text_str);
    while(text_str[len] == ' ' || text_str[len] == '\0')
        len--;
    text_str[++len] = '\0';
    
    strcpy(sql_text, text_str);

    return sqlca.sqlcode;
}

int sql_select (slctstmt)
EXEC SQL BEGIN DECLARE SECTION;
    char *slctstmt;
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
     /* FILE *unlfile; */
     /* char delim = '|'; */
     char delim = ' ';
     char *buffer = NULL;

     EXEC SQL prepare usqlobj from :slctstmt;
     if ( sqlca.sqlcode != 0 ) 
     {
	  fprintf(stderr, "sql error! [%d]\n", sqlca.sqlcode);
	  return -1;
     }
     EXEC SQL describe usqlobj into udesc;
     if ( sqlca.sqlcode != 0 ) 
     {
	  fprintf(stderr, "sql error! [%d]\n", sqlca.sqlcode);
	  return -1;
     }
     
     /** unlfile = fopen(fname,"w");
     if ( unlfile == NULL )
	  {
	  fprintf(stderr,"Cannot open file '%s'\n", fname);
	  return -1;
	  } **/

     /* Step 1: analyze udesc to determine type and memory requirements
      *         of each item in the select list.  rtypalign() returns a
      *         pointer to the next appropriate boundary (starting at
      *         pos) for the indicated data type.
      */
fprintf(stdout, "**********************************\n");
     pos = 0;
     for (col = udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	  {
fprintf (stdout, "%s ", col->sqlname);
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
		     fprintf (stderr, "Don't support this type: %d", col->sqltype);
		    return -1;
		}
	col->sqllen = rtypmsize(col->sqltype,col->sqllen);
	pos = rtypalign( pos, col->sqltype) + col->sqllen;
	col->sqlind = NULL;
	}
fprintf (stdout, "\n" );

     /* Step 2: Allocate memory to receive a row of the table returned
      *         by the SELECT statement.  The variable pos has an integer
      *         value equal to the number of bytes occupied by the row.
      */
     buffer = malloc(pos);
     if ( buffer == NULL )
	   {
	   fprintf(stderr,"Out of memory\n");
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

    if ( has_trans )
         EXEC SQL begin work;

    EXEC SQL open usqlcurs;
    EXEC SQL fetch usqlcurs using descriptor udesc;
fprintf(stdout, "--------------------------------------\n");
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
	       /** fputs(fieldp,unlfile); 
	         putc(delim,unlfile); */
	       fprintf (stdout, "%s%c", fieldp, delim);
	    }
	    /* putc('\n',unlfile); */
	    fprintf (stdout, "\n");

        EXEC SQL fetch usqlcurs using descriptor udesc;
	}

     /* fclose(unlfile); */
     free(buffer);
     if ( sqlca.sqlcode && sqlca.sqlcode != SQLNOTFOUND ) 
	 {
	  /* display ERROR message	*/
	  fprintf(stderr,"select failed sqlcode = %d\n", sqlca.sqlcode);
	 }
     else 
	 {
	  fprintf(stderr,"%d rows select\n" , sqlca.sqlerrd[2]);
	 }
     fprintf(stdout, "**********************************\n");
     
     EXEC SQL close usqlcurs;
     if ( has_trans )
        EXEC SQL commit work;
	free(udesc);
	
	return 0;
}
