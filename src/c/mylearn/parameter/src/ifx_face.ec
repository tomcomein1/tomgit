EXEC SQL include ifx_face.h;

/*�����ݿ�*/
int db_ifx_open( )
{  
    EXEC SQL CONNECT TO :cnname AS :svrname USER :usrname USING:passwd;
    return sqlca.sqlcode;
} 

/*�ر����ݿ�*/
int db_ifx_close( )
{
    EXEC SQL DISCONNECT :cnname;
    return sqlca.sqlcode;
}

/* �ύ���� */  
int db_ifx_commit( )  
{  
    /* �ύ���� */  
    EXEC SQL COMMIT WORK;  
    return sqlca.sqlcode;
}  

/* �ع����� */  
int db_ifx_rollback( )  
{  
    /* �ع����� */  
    EXEC SQL ROLLBACK WORK;  
    return sqlca.sqlcode;
}

/* ִ�зǲ�ѯ�Ķ�̬SQL��� */  
int db_ifx_nquery(const char *sql)  
{  
EXEC SQL BEGIN DECLARE SECTION;  
    char sql_stmt[SQL_STMT_MAX_LEN] = {0};  
    int ret = 0;
EXEC SQL END DECLARE SECTION;  
  
    snprintf(sql_stmt, sizeof(sql_stmt), "%s", sql);  
    
    EXEC SQL begin work;
    /* ׼����̬SQL��� */  
    EXEC SQL PREPARE NQUERY_SQLSTMT_ID FROM :sql_stmt;  
    
    /* ִ�У��ǲ���SQL */  
    EXEC SQL EXECUTE NQUERY_SQLSTMT_ID ;
    if (sqlca.sqlcode == 0)
        ret = db_ifx_commit();
    else
    {
        ret = sqlca.sqlcode;
        db_ifx_rollback();
    }

    return ret;
}

