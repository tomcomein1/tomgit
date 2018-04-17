EXEC SQL include ifx_face.h;

/*打开数据库*/
int db_ifx_open( )
{  
    EXEC SQL CONNECT TO :cnname AS :svrname USER :usrname USING:passwd;
    return sqlca.sqlcode;
} 

/*关闭数据库*/
int db_ifx_close( )
{
    EXEC SQL DISCONNECT :cnname;
    return sqlca.sqlcode;
}

/* 提交事务 */  
int db_ifx_commit( )  
{  
    /* 提交事务 */  
    EXEC SQL COMMIT WORK;  
    return sqlca.sqlcode;
}  

/* 回滚事务 */  
int db_ifx_rollback( )  
{  
    /* 回滚事务 */  
    EXEC SQL ROLLBACK WORK;  
    return sqlca.sqlcode;
}

/* 执行非查询的动态SQL语句 */  
int db_ifx_nquery(const char *sql)  
{  
EXEC SQL BEGIN DECLARE SECTION;  
    char sql_stmt[SQL_STMT_MAX_LEN] = {0};  
    int ret = 0;
EXEC SQL END DECLARE SECTION;  
  
    snprintf(sql_stmt, sizeof(sql_stmt), "%s", sql);  
    
    EXEC SQL begin work;
    /* 准备动态SQL语句 */  
    EXEC SQL PREPARE NQUERY_SQLSTMT_ID FROM :sql_stmt;  
    
    /* 执行：非查找SQL */  
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

