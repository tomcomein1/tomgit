EXEC SQL include "dbtools.h";
EXEC SQL include "para_macro.h";
EXEC SQL include "ifx_face.h";

/**参数设置业务逻辑处理函数 **/
int ParamSet(char *in, char *out)
{
     int ret = 0;

     if (strstr(in, "DBPWDCHECK") == NULL)
         WriteLog(ERR_LVL, "in:[%s]", in);

     ret = para_set_busi(in, out);
     return 0;
}

int para_set_busi(char *in, char *out)
{
    char sql[MAX_SQL_LEN];
    char file[MAX_FILE_LEN];
    char filename[MAX_FILE_LEN];
    char tabname[MAX_FILE_LEN];
    char retmsg[MAX_BUFF_LEN];
    char trancode[20];
    int ret = 0;

    memset(retmsg, 0, MAX_BUFF_LEN);
    memset(file, 0, MAX_FILE_LEN);

    memset(trancode, 0, sizeof(trancode) );
    if (get_xml_content(in, "trancode", trancode) == NULL)
    {
         strcpy(ErrInfo, "not found label: <trancode>");
         goto err_point;
    }

    if (strncmp(trancode, "DBPWDCHECK", 10) == 0)
    {
        ret = check_connect(in); 
    }
    else if (strncmp(trancode, "BACKUP", 6) == 0)
    {
        memset(filename, 0, MAX_FILE_LEN);
        if (get_xml_content(in, "filename", filename) == NULL)
        {
             strcpy(ErrInfo, "not found label: <filename>");
             goto err_point;
        }

        sprintf(file, "%s/%s/%s", getenv("HOME"), PARA_BACKUP_DIR, filename );
        WriteLog(ERR_LVL, "file=[%s]", file);

        memset(sql, 0, MAX_SQL_LEN);
        if (get_xml_content(in, "sql", sql) == NULL)
        {
             strcpy(ErrInfo, "not found label: <sql>");
             goto err_point;
        }

         strcpy (cnname, (char *) getenv("INIT_USING_DBASE") );
         ret = connect_database(cnname) ;
         WriteLog(ERR_LVL, "connect db:[%s]ret[%d]", cnname, ret );
         if (ret == 0)
         {
             ret = exec_sql (sql, file, retmsg);
             close_database();
             WriteLog(ERR_LVL, "close db:[%s]ret[%d]", cnname, ret );
         }
    }
    else if (strncmp(trancode, "RESTORE", 7) == 0)
    {
        memset(filename, 0, MAX_FILE_LEN);
        if (get_xml_content(in, "filename", filename) == NULL)
        {
             strcpy(ErrInfo, "not found label: <filename>");
             goto err_point;
        }

        /** 取表名 **/
        memset(tabname, 0, MAX_FILE_LEN);
        if (get_xml_content(in, "tablename", tabname) == NULL)
        {
             strcpy(ErrInfo, "not found label: <tablename>");
             goto err_point;
        }

        sprintf(file, "%s/%s/%s", getenv("HOME"), PARA_BACKUP_DIR, filename );
        WriteLog(ERR_LVL, "file=[%s] tabname=[%s]", file, tabname);

         strcpy (cnname, (char *) getenv("INIT_USING_DBASE") );
         ret = connect_database(cnname) ;
         WriteLog(ERR_LVL, "connect db:[%s]ret[%d]", cnname, ret );
         if (ret == 0)
         {
             ret = para_restore(file, tabname );
             close_database();
             WriteLog(ERR_LVL, "close db:[%s]ret[%d]", cnname, ret );
         }
    }
    else if (strncmp(trancode, "test", 4) == 0)
    {
         strcpy (cnname, (char *) getenv("INIT_USING_DBASE") );
         ret = connect_database(cnname) ;
         WriteLog(ERR_LVL, "connect db:[%s]ret[%d]", cnname, ret );
         if (ret == 0)
         {
             /* ret = QuerySignInfo(in, retmsg); */
             close_database();
             WriteLog(ERR_LVL, "close db:[%s]ret[%d]", cnname, ret );
         }
    }
    else
    {
         memset(sql, 0, MAX_SQL_LEN);
         if (get_xml_content(in, "sql", sql) == NULL)
        {
             strcpy(ErrInfo, "not found label: <sql>");
             goto err_point;
        }

WriteLog(ERR_LVL, "sql[%s]", sql);

         strcpy (cnname, (char *) getenv("INIT_USING_DBASE") );
         ret = connect_database(cnname) ;
         WriteLog(ERR_LVL, "connect db:[%s]ret[%d]", cnname, ret );
         if (ret == 0)
         {
             ret = exec_sql (sql, file, retmsg);
             close_database();
             WriteLog(ERR_LVL, "close db:[%s]ret[%d]", cnname, ret );
         }
    }

    if (ret != 0)
    {
err_point:
         if (ErrInfo[0] == '\0') sprintf(ErrInfo, "%s falsed", trancode);
         sprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
         sprintf(out+strlen(out), "<head><trancode>%s</trancode></head>\n", trancode);
         sprintf(out+strlen(out), "<body>\n<code>%ld</code>\n", ret);
	 sprintf(out+strlen(out), "<msg>%s</msg>\n</body>", ErrInfo);
    }
    else 
    {
         if (retmsg[0] == '\0')  sprintf(retmsg, "%s succeed", trancode);
         sprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
         sprintf(out+strlen(out), "<head><trancode>%s</trancode></head>\n", trancode);
         sprintf(out+strlen(out), "<body>\n<code>0</code>\n");
	 sprintf(out+strlen(out), "<msg>%s</msg>\n</body>", retmsg);
    }

    return ret;
}

/** 检查数据库用户密码连接 **/
int check_connect(char *in)
{
     int ret = 0;

     memset(svrname, 0, sizeof(svrname) );
     memset(usrname, 0, sizeof(usrname) );
     memset(passwd, 0, sizeof(passwd) );
     memset(cnname, 0, sizeof(cnname) );

     strcpy(svrname, (char *) getenv("INFORMIXSERVER") );
     strcpy(cnname, (char *) getenv("INIT_USING_DBASE") ); 
     if (get_xml_content(in, "dbname", usrname) == NULL)
     {
        strcpy (ErrInfo, "未找到dbname标签"); 
        return -1;
     }
     if (get_xml_content(in, "passwd", passwd) == NULL)
     {
        strcpy (ErrInfo, "未找到passwd标签");
        return -1;
     }

     ret = db_ifx_open(); 
WriteLog(ERR_LVL, "svrname[%s]cnname[%s]usrname[%s]passwd[******]sqlca.sqlcode[%d]", svrname, cnname, usrname, ret);
     if (ret == 0) db_ifx_close();
     else strcpy (ErrInfo, "验证用户密码错.");

     return ret;
}

