// name： opendbsqlite.c
// This prog is used to test C/C++ API for sqlite3.It is very simple,ha!
// Author : zieckey All rights reserved.
// data : 2006/11/13
#include <sqlite3.h>
#include <stdio.h>
int main (void)
{
  sqlite3 *db = NULL;
  char *zErrMsg = 0;
  int rc;
  //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件

  rc = sqlite3_open ("zieckey.db", &db);
  if (rc)
    {
      fprintf (stderr, "Can't open database: %s ", sqlite3_errmsg (db));
      sqlite3_close (db);
      exit (1);
    }
  else
    printf ("You have opened a sqlite3 database named zieckey.db successfully! \nCongratulations! Have fun ! ^-^ \n");
  sqlite3_close (db);		//关闭数据库
  return 0;
}

