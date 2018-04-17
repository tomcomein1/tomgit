/*------------------------------------------------------------------------

                     Server for Remote Database Access
                     
                         远程数据库访问服务端程序

                          
程序员：陈博
制作时间：2000.10.08



---------------------------------------------------------------------------*/



#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

#include"netmain.h"
#include"dlpublic.h"
#include"dlywd.h"
#include"rda.h"

long dsTaskRunSql(int nInSocket,long nTaskNo)
{
     char sDestination[51];
     char sGateWay[51];
     char sSql[MAX_SQL_SIZE];
     long nRouteNum;
     long nRetVal,nError;
     int  nOutSocket;
  
     if(GetFromSocket(nInSocket,"%s%ld",sDestination,&nRouteNum))
     {
       
        SendLongToSocket(nInSocket,-1);
        return 0;
     }
  
     if(nRouteNum<=0)
     {
     	SendLongToSocket(nInSocket,-2);
     	return 0;
     }
 
     if(FindRoute(sDestination,sGateWay)<=0)
     {
        // 没有缺省路由
        SendLongToSocket(nInSocket, -3);
        return 0;
     }     	

        // 没有目的地址的路由，使用缺省路由
     
     if(strcmp(sGateWay,"localhost"))   //如果目标主机不是本机
     {
     	  
        if(nRouteNum<=0)
        {
            // 路由数超出
            SendLongToSocket(nInSocket, -4);
            return 0;
        }     	
        
        nOutSocket = ConnectRemote(sGateWay,"dlyw");
        if(nOutSocket < 0)
        {
              if(SendLongToSocket(nInSocket,-5)) return 0;
              return 0;
        }

        nError=SendToSocket(nOutSocket, "%ld%s%ld",
                            nTaskNo,sDestination,nRouteNum-1);
        if(!nError) nError=GetLongFromSocket(nOutSocket,&nRetVal);
        
        if(!nError) nError=SendLongToSocket(nInSocket,nRetVal);
        
        if(nError || nRetVal!=0)
        {
              DisconnectRemote(nOutSocket);
              return 0;
        }      
        
        ///////////////////////////进入中转过程///////////////////////
        for(;;)
        {
              nError=GetLongFromSocket(nInSocket,&nRetVal);
              if(!nError) nError=SendLongToSocket(nOutSocket,nRetVal);
              if(nError||nRetVal==0)
              {
                   DisconnectRemote(nOutSocket);
                   return 0;
              }

              nError=GetStringFromSocket(nInSocket,sSql); 
              if(!nError)  nError=SendStringToSocket(nOutSocket,sSql); 
              printf("\nsend to.... %s",sSql);
              if(nError)
              {
                   DisconnectRemote(nOutSocket);
                   return 0;
              }
              GetLongFromSocket(nOutSocket,&nRetVal);
              SendLongToSocket(nInSocket,nRetVal);
              if(nRetVal<0)
              {
              	   GetStringFromSocket(nOutSocket,sSql);
              	   SendStringToSocket(nInSocket,sSql);
              }	   

        } //end for     
     }         	     
     else               //已经是本地机
     {
        //达到了目的地，通知客户机准备就绪可以执行SQL语名了
        if(SendLongToSocket(nInSocket,0)) return 0;
        for(;;)
        {
        	
            if(GetLongFromSocket(nInSocket,&nRetVal)) return 0;
            switch(nRetVal)
            {
              case 0:
                 return 0;
            
              case 1:   //Run Sql  

                 if(GetStringFromSocket(nInSocket,sSql)) return 0;
                 printf("\n执行远程SQL语句：%s\n",sSql);
                 nRetVal=RunSql(sSql);
                 if(SendLongToSocket(nInSocket,nRetVal)) return 0;
                 if(nRetVal<0)
                 {
                     GetSqlErrInfo(sSql);
                     if(SendStringToSocket(nInSocket,sSql)) return 0;
                 }    	
                 break;
            } //end switch        
        }//end for(;;)
    }//end else    

}

long dsTaskQueryRemote(int nInSocket, long nTaskNo)
{
  char sDestination[51];
  char sGateWay[51];
  char sSql[MAX_SQL_SIZE];
  char * sFormat;
  long nRetVal,nLoop,nError;
  long nRouteNum,nId;
  int nOutSocket;

  QUERYUNIT query;     //查询库查询结构
    

  if(GetFromSocket(nInSocket,"%s%s%s%ld",sDestination,query.sFormat,sSql,&nRouteNum))
  {
        return 1;
  }
  
  sFormat=query.sFormat;
  for(nLoop=0;nLoop<ITEMSIZE;nLoop++) query.data[nLoop].pLong=NULL;
  nLoop=0;
  nError=0;
    

/////////////////////////    数据动态分配        //////////////////////////////        
  while(sFormat=strchr(sFormat,'%'))
  {
    	sFormat++;
    	switch(sFormat[0])
    	{
    	     case 's':
    	          nError=((query.data[nLoop].pChar=(char *)malloc(STRINGSIZE))==NULL);
    	          break;
    	     case 'l':
    	          sFormat++;
    	          switch(sFormat[0])
    	          {
    	          	case 'd':
          	             nError=((query.data[nLoop].pLong=
          	                            (long *)malloc(sizeof(long)))==NULL);
    	                     break;
    	     		case 'f':
    	          	     nError=((query.data[nLoop].pDouble=
    	          	                    (double *)malloc(sizeof(double)))==NULL);
    	          	     break;
    	          }
    	          break;
    	}
    	if(nError||(nLoop++)>ITEMSIZE)   //动态分配内存出错
    	{

                SendToSocket(nInSocket,"%ld%s",-1,"数据动态分配失败");  	
                ReleaseQueryUnit(&query);
    		return -2;
    	}
     }//end while


////////////////////////////  路由选择  ////////////////////////////////////


     if(FindRoute(sDestination,sGateWay)<=0)
     {
        // 没有缺省路由
        sprintf(sSql,"没有通向%s的路由!",sDestination);
        SendToSocket(nInSocket,"%ld%s", -3,sSql);
        ReleaseQueryUnit(&query);
        return 0;
     }     	
     
     if(strcmp(sGateWay,"localhost"))   //如果目标主机不是本机
     {
     	  
        if(nRouteNum<=0)
        {
           SendToSocket(nInSocket,"%ld%s",-4,"路由数超出最大限制！");
           ReleaseQueryUnit(&query);
           return 0;
        }     	
        
        nOutSocket = ConnectRemote(sGateWay,"dlyw");
        if(nOutSocket < 0)
        {
              ReleaseQueryUnit(&query);
              sprintf(sSql,"无法%s与的dlywd服务建立连接!",sGateWay);
              SendToSocket(nInSocket,"%ld%s",-5,sSql);
              return 0;
        }
        
        nError=SendToSocket(nOutSocket, "%ld%s",nTaskNo,sDestination);
        if(!nError) nError=SendStringToSocket(nOutSocket,query.sFormat);
        if(!nError) nError=SendStringToSocket(nOutSocket,sSql);
        if(!nError) nError=SendLongToSocket(nOutSocket,nRouteNum-1);
        
        if(!nError) nError=GetLongFromSocket(nOutSocket,&nRetVal);
        
        if(!nError) nError=SendLongToSocket(nInSocket,nRetVal);
        
        if(nError)
        {
              ReleaseQueryUnit(&query);
              SendToSocket(nInSocket,"%ld%s",-6,"网络出错！");
              return 0;
        }      
        
        if(nRetVal<0)
        {
              GetStringFromSocket(nOutSocket,sSql);
              SendToSocket(nOutSocket,"%ld%s",-7,sSql);
              DisconnectRemote(nOutSocket);
              ReleaseQueryUnit(&query);
              return 0;
              
        }
        	
        
        
        ///////////////////////////进入中转过程/////////////////////////////////
        for(;;)
        {
              nError=GetLongFromSocket(nInSocket,&nRetVal);
              if(!nError) nError=SendLongToSocket(nOutSocket,nRetVal);
              if(nError||nRetVal==0)
              {
                   ReleaseQueryUnit(&query);
              	   SendLongToSocket(nOutSocket,0);
                   DisconnectRemote(nOutSocket);
                   return 0;
              }

              nError=GetLongFromSocket(nOutSocket,&nRetVal); 
              if(!nError)  nError=SendLongToSocket(nInSocket,nRetVal); 
              if(nError||nRetVal==0)
              {
                   DisconnectRemote(nOutSocket);
                   ReleaseQueryUnit(&query);
                   return 0;
              }

              nError=GetStructionFromServer(nOutSocket,&query);
              
              if(!nError) nError=SendStructionToClient(nInSocket,&query);
              
              if(nError)
              {
                   ReleaseQueryUnit(&query);
              	   SendLongToSocket(nOutSocket,0);
                   DisconnectRemote(nOutSocket);
                   return 0;
              }
        } //end for     
     }         	     
     else               //已经是本地机
     {
     	printf("\n打开远程游标:%s\n",sSql);
     	nId=OpenCursor(sSql);
 	if(SendLongToSocket(nInSocket,nId))
 	{
                if(nId>0) CloseCursor(nId);
                ReleaseQueryUnit(&query);
                return 0;
 	}
 	
 	if(nId<=0)
     	{	
                ReleaseQueryUnit(&query);
                GetSqlErrInfo(sSql);
                SendStringToSocket(nInSocket,sSql);
                printf("远程游标打开错误！:%s\n",sSql);
     		return 0;
        }
                       	
     	//////////////////////////////////取得数据//////////////////////////////
     	
     	for(;;)
     	{
                
                nError=GetLongFromSocket(nInSocket,&nRetVal);   //询问客户端是否需要数据
                if(nError||nRetVal==0)                           //如果不需要则终止进程
                {
                       ReleaseQueryUnit(&query);
              	       CloseCursor(nId);
     		       return 0;
     		}       

                nRetVal=FetchStruction(nId,&query);      //取得数据
                nError=SendLongToSocket(nInSocket,(nRetVal!=0));     //响应客户端
                if(nError||nRetVal==0) 
     	        {
                       ReleaseQueryUnit(&query);
              	       CloseCursor(nId);
     		       return 0;
     		}

     		if(SendStructionToClient(nInSocket,&query))  //向客户端发数据
     		{
                        ReleaseQueryUnit(&query);
           	        CloseCursor(nId);
     			return 0;
     		}

	}//end for(;;)

     }//end else
}


long dsTaskOpenRemoteCursor(int nInSocket, long nTaskNo)
{
     char sBuffer[CURSOR_PACKAGE_SIZE];
     char sDestination[51];
     char sGateWay[51];
     char sSql[MAX_SQL_SIZE];
     char * sFormat,* sOffset;
     long nRetVal,nLoop,nError;
     long nRouteNum,nId,nsequence,nBytes=0;
     int  nOutSocket;
     int  fd_flags;
     QUERYUNIT query;     //查询库查询结构
    

     fd_flags=fcntl(nInSocket,F_GETFL,0);
     if(fd_flags!=-1)
     {
     	fd_flags|=O_NONBLOCK;
     	fcntl(nInSocket,F_SETFL,fd_flags);
     }	
     	
     if(GetFromSocket(nInSocket,"%s%s%s%ld",sDestination,query.sFormat,sSql,&nRouteNum))
     {
        return 0;
     }
  

     /////////////////////////  路由选择  ////////////////////////
     if(FindRoute(sDestination,sGateWay)<=0)
     {
        // 没有缺省路由
        sprintf(sSql,"找不到%s的路由！",sDestination);
        SendToSocket(nInSocket,"%ld%s",-3,sSql);
        return 0;
     }     	
     
     if(strcmp(sGateWay,"localhost"))   //如果目标主机不是本机
     {
     	  
        CloseDatabase();
        if(nRouteNum<=0)
        {
        // 没有缺省路由
            SendToSocket(nInSocket, "%ld%s",-4,"路由数超出最大限制！");
            return 0;
        }     	
                        
   
        nOutSocket = ConnectRemote(sGateWay,"dlyw");
        printf("\n将文件游标任务路由到%s",sGateWay);
        if(nOutSocket < 0)
        {
              sprintf(sSql,"无法与%s的dlywd服务建立连接!",sGateWay);
              if(SendToSocket(nInSocket,"%ld%s",-5,sSql)) return 0;
              return 0;
        }

        
        nError=SendToSocket(nOutSocket, "%ld%s",nTaskNo,sDestination);
        if(!nError) nError=SendStringToSocket(nOutSocket,query.sFormat);
        if(!nError) nError=SendStringToSocket(nOutSocket,sSql);
        if(!nError) nError=SendLongToSocket(nOutSocket,nRouteNum-1);
        
        
        if(!nError) nError=GetLongFromSocket(nOutSocket,&nRetVal);
        
        if(!nError) nError=SendLongToSocket(nInSocket,nRetVal);
        
        if(nError)
        {
              DisconnectRemote(nOutSocket);
              SendToSocket(nInSocket,"%ld%s",-6,"网络出错！");
              return 0;
        }      
        
        if(nRetVal<0)
        {
              GetStringFromSocket(nOutSocket,sSql);
              DisconnectRemote(nOutSocket);
              SendToSocket(nInSocket,"%ld%s",-7,sSql);
              return 0;
        }
                
        nError=GetLongFromSocket(nInSocket,&nRetVal);
        if(!nError) nError=SendLongToSocket(nOutSocket,nRetVal);
        if(nError||nRetVal==0)
        {
              DisconnectRemote(nOutSocket);
              return 0;
        }
        
        ///////////////////////////进入中转过程/////////////////////////////////
        for(;;)
        {
        	if(GetStringFromSocket(nOutSocket,sBuffer)) return 0;
        	if(SendStringToSocket(nInSocket,sBuffer))   return 0;
        	if(strstr(sBuffer,"SUCCESS_FETCH_CURSOR")||
 	                  strstr(sBuffer,"FAILED_FETCH_CURSOR"))
 	        {
 	        	DisconnectRemote(nOutSocket);
 	        	return 0;
 	        }
 	        
        } //end for     
     }         	     
     else               //已经是本地机
     {

	sFormat=query.sFormat;
  	for(nLoop=0;nLoop<ITEMSIZE;nLoop++) query.data[nLoop].pLong=NULL;
  	nLoop=0;
  	nError=0;
    

  	///////////////////    数据动态分配     ///////////////////
  	while(sFormat=strchr(sFormat,'%'))
  	{
    	   sFormat++;
    	   switch(sFormat[0])
    	   {
    	     case 's':
    	          nError=((query.data[nLoop].pChar=(char *)malloc(STRINGSIZE))==NULL);
    	          break;
    	     case 'l':
    	          sFormat++;
    	          switch(sFormat[0])
    	          {
    	          	case 'd':
          	             nError=((query.data[nLoop].pLong=
          	                            (long *)malloc(sizeof(long)))==NULL);
    	                     break;
    	     		case 'f':
    	          	     nError=((query.data[nLoop].pDouble=
    	          	                    (double *)malloc(sizeof(double)))==NULL);
    	          	     break;
    	          }
    	          break;
    	    }
    	    if(nError||(nLoop++)>ITEMSIZE)   //动态分配内存出错
    	    {

                SendLongToSocket(nInSocket,-1);  	
                ReleaseQueryUnit(&query);
    		return 0;
    	    }
        }//end while

     	printf("\n打开远程文件游标:%s\n",sSql);
     	nId=OpenCursor(sSql);
 	if(SendLongToSocket(nInSocket,nId))
 	{
                if(nId>0) CloseCursor(nId);
                ReleaseQueryUnit(&query);
                return 0;
 	}
 	
 	if(nId<=0)
     	{	
                ReleaseQueryUnit(&query);
                GetSqlErrInfo(sSql);
                SendStringToSocket(nInSocket,sSql);
                printf("远程游标打开错误！%s\n",sSql);
     		return 0;
        }
                       	
     	//////////////////////////////////取得数据//////////////////////////////
     	
        nError=GetLongFromSocket(nInSocket,&nRetVal);   //询问客户端是否需要数据
        if(nError||nRetVal==0)                           //如果不需要则终止进程
        {
             ReleaseQueryUnit(&query);
             CloseCursor(nId);
     	     return 0;
     	}       
        
        sOffset=sBuffer;
        nsequence=0;
     	for(;;)
     	{
             nError++;
             nRetVal=FetchStruction(nId,&query);      //取得数据
             sFormat=query.sFormat;
             if(nRetVal>0)
             {
             	nLoop=0;
      		while(sFormat=strchr(sFormat,'%'))
  		{
                          	   	   
    	   	    sFormat++;
    	   	    switch(sFormat[0])
    	   	    {
    	     		case 's':
    	          	    sprintf(sOffset,"%s",query.data[nLoop].pChar);
    	          	    break;
    	     		case 'l':
    	          	    sFormat++;
    	          	    switch(sFormat[0])
    	          	    {
    	          	        case 'd':
      	          	          sprintf(sOffset,"%ld",(* query.data[nLoop].pLong));
    	                        break;
    	     		    
    	     		        case 'f':
      	          	          sprintf(sOffset,"%lf",(* query.data[nLoop].pDouble));
    	          	        break;
        	            }
    	                    break;
    	           }
                   while(sOffset[0]!='\0') sOffset++;
    	           sprintf(sOffset,"|\0");
    	           sOffset++;
    	           nLoop++;
    	        }//end while
                sOffset[0]='\n';
                sOffset[1]='\0';
                sOffset++;
             }//end if
             
             if(nRetVal==0) strcpy(sOffset,"SUCCESS_FETCH_CURSOR\n");
             if(nRetVal<0)  strcpy(sOffset,"FAILED_FETCH_CURSOR\n");
             
             if(nRetVal<=0||(sOffset-sBuffer)>CURSOR_SEND_SIZE)
             {
                
//             	printf("\nsequence=%ld\n%s",nsequence++,sBuffer);
                
             	SendStringToSocket(nInSocket,sBuffer);
                sOffset=sBuffer;
                nsequence++;
                nBytes+=strlen(sBuffer);
                memset(sBuffer,'\0',sizeof(sBuffer));
             	if(nRetVal<=0)
             	{
             	      CloseCursor(nId);
                      CloseDatabase();
                      ReleaseQueryUnit(&query);
                      printf("\n%ld packages,%ld bytes transmitted sucessfully.",nsequence,nBytes);
             	      return 0;
             	}
             }
       
	}//end for(;;)

     }//end else
}


long dsTaskFetchFileCursor(int nInSocket, pid_t child)
{
     char sBuffer[CURSOR_PACKAGE_SIZE];
     char sCursorFile[31];
     long nRetVal,nLoop,nError;
     long nSize=0;
     FILE * fpr;
     struct stat file_stat;
    

     if(GetStringFromSocket(nInSocket,sCursorFile))
     {
        close(nInSocket);
     	return 0;
     }
     if(access(sCursorFile,0))
     {
     	SendToSocket(nInSocket,"%ld%s",-1,"游标文件不存在！");
        close(nInSocket);
     	return 0;
     }
     
     if(!(fpr=fopen(sCursorFile,"rt")))
     {
     	SendToSocket(nInSocket,"%ld%s",-2,"无法打开游标文件！");
        close(nInSocket);
     	return 0;
     }
     SendLongToSocket(nInSocket,1);

     for(;;)
     {
     	for(;;)
     	{
     		stat(sCursorFile,&file_stat);
     		if(file_stat.st_size>(nSize+1)) break;
     		sleep(1);
  	}
     	
     	nRetVal=getline(sBuffer,fpr);
        if(nRetVal>0) nError=SendStringToSocket(nInSocket,sBuffer);
     	if(nRetVal<=0||nError)
     	{
     	
     		fclose(fpr);
                close(nInSocket);
     		return 0;
     	}
 	if(strstr(sBuffer,"SUCCESS_FETCH_CURSOR")||strstr(sBuffer,"FAILED_FETCH_CURSOR"))
 	{
 		fclose(fpr);
                close(nInSocket);
     		sprintf(sBuffer,"rm %s",sCursorFile);
     		system(sBuffer);
     		return 0;
        }
        nSize+=strlen(sBuffer);     	
     }//end for     	

}

long dsTaskOpenFileCursor(int nInSocket, long nTaskNo)
{
     char sBuffer[CURSOR_PACKAGE_SIZE];
     char sCursorFile[31];
     char sDestination[51];
     char sGateWay[51];
     char sSql[2000];
     char sFormat[ITEMSIZE*3];
     long nRetVal,nLoop,nError,nRouteNum;
     long nsequence=0,nbytes=0;
     int nOutSocket;
     pid_t child;
     FILE * fpw;
    

     if(GetFromSocket(nInSocket,"%s%s%s%ld",sDestination,sFormat,sSql,&nRouteNum))
     {
        close(nInSocket);
        return 1;
     }
  
     ////////////////////  路由选择  /////////////////////
     if(FindRoute(sDestination,sGateWay)<=0)
     {
        // 没有缺省路由
        sprintf(sSql,"没有%s的路由!",sDestination);
        SendToSocket(nInSocket, "%ld%s",-3,sSql);
        close(nInSocket);
        return 0;
     }     	
     
     nOutSocket = ConnectRemote(sGateWay,"dlyw");
     if(nOutSocket < 0)
     {
     	 sprintf(sSql,"不能与%s的dlywd服务建立连接",sGateWay);
         SendToSocket(nInSocket,"%ld%s",-4,sSql);
         close(nInSocket);
         return 0;
     }
     nError=SendToSocket(nOutSocket, "%ld%s",TASK_OPEN_REMOTECURSOR,sDestination);
     if(!nError) nError=SendStringToSocket(nOutSocket,sFormat);
     if(!nError) nError=SendStringToSocket(nOutSocket,sSql);
     if(!nError) nError=SendLongToSocket(nOutSocket,nRouteNum-1);

     if(!nError) nError=GetLongFromSocket(nOutSocket,&nRetVal);
     if(!nError) nError=SendLongToSocket(nInSocket,(nRetVal>0)?1:-1);
        
     if(nError)
     {
         SendToSocket(nInSocket,"%ld%s",-5,"网络出错！");
         close(nInSocket);
         return 0;
     }      
   
     if(nRetVal<0)
     {
     	 GetStringFromSocket(nOutSocket,sSql);
     	 SendStringToSocket(nInSocket,sSql);
     	 DisconnectRemote(nOutSocket);
     	 close(nInSocket);
     }
     	
     	 

     RunSelect("select to_number(to_char(sysdate,'hh24miss'))*100 from dual into %ld",&nLoop);
     CloseDatabase();     //以后不需要数据库支持，关闭数据库以释放ORACLE资源
     GetTempFileName(sCursorFile);
     if(!(fpw=fopen(sCursorFile,"wb")))
     {
     	 SendLongToSocket(nOutSocket,0);
     	 DisconnectRemote(nOutSocket);
         close(nInSocket);
     	 return 0;
     }
     if(nRetVal>0) SendStringToSocket(nInSocket,sCursorFile);
     	
     // 此时与客户机的通讯完毕，开始接收服务端的数据 

     //Create a child process to communicate with the server
     //and the parent process to communicate with the client
     child=fork();

     
     if(child>0)
     {
     	close(nOutSocket);
     	fclose(fpw);
     	dsTaskFetchFileCursor(nInSocket,child);
     	exit(0);
     }
  
     close(nInSocket);
     SendLongToSocket(nOutSocket,1);
     printf("\n接收文件游标数据...\n");

     for(;;)
     {
     	memset(sBuffer,'\0',sizeof(sBuffer));
     	if(GetStringFromSocket(nOutSocket,sBuffer))
     	{
     		strcpy(sBuffer,"FAILED_FETCH_CURSOR\n");
     		fwrite(sBuffer,strlen(sBuffer),1,fpw);
     		fclose(fpw);
                DisconnectRemote(nOutSocket);
                close(nInSocket);
     		return 0;
     	}
 	fwrite(sBuffer,strlen(sBuffer),1,fpw);
        fflush(fpw);    //将接收到的字符流写入磁盘文件，但不关闭文件
        nsequence++;
        nbytes+=strlen(sBuffer);
         
 	if(strstr(sBuffer,"SUCCESS_FETCH_CURSOR")||strstr(sBuffer,"FAILED_FETCH_CURSOR"))
 	{   

 		fclose(fpw);
 		DisconnectRemote(nOutSocket);
                close(nInSocket);
                printf("\n%ld packages,%ld bytes received successfully...",nsequence,nbytes);
 		return 0;
 	}

     }		

}



//从上级服务器上取得数据
int GetStructionFromServer(int nSocket,QUERYUNIT * pQuery)
{

     char * sFormat;
     long nLoop,nError=0;

     if(nSocket<=0 || pQuery==NULL) return 1;
     
     sFormat=pQuery->sFormat;
     nLoop=0;
     while(sFormat=strchr(sFormat,'%'))
     {
    	sFormat++;
    	switch(sFormat[0])
    	{
    	     case 's':
    	          nError=GetStringFromSocket(nSocket,pQuery->data[nLoop].pChar);
    	          break;
    	     case 'l':
    	          sFormat++;
    	          switch(sFormat[0])
    	          {
    	          	case 'd':
          	             nError=GetLongFromSocket(nSocket,pQuery->data[nLoop].pLong);
    	                     break;
    	     		case 'f':
    	          	     nError=GetDoubleFromSocket(nSocket,pQuery->data[nLoop].pDouble);
    	          	     break;
    	          }
    	          break;
    	}
    	if(nError) return 1;
        nLoop++;
     }//end while
     return 0;


}

//将取得的数据发往客户机

int SendStructionToClient(int nSocket,QUERYUNIT * pQuery)
{
  
     char * sFormat;
     long nLoop,nError=0;

     if(nSocket<=0 || pQuery==NULL) return 1;
     
     sFormat=pQuery->sFormat;
     nLoop=0;
     while(sFormat=strchr(sFormat,'%'))
     {
    	sFormat++;
    	switch(sFormat[0])
    	{
    	     case 's':
    	          nError=SendStringToSocket(nSocket,pQuery->data[nLoop].pChar);
    	          break;
    	     case 'l':
    	          sFormat++;
    	          switch(sFormat[0])
    	          {
    	          	case 'd':
          	             nError=SendLongToSocket(nSocket,(* pQuery->data[nLoop].pLong));
    	                     break;
    	     		case 'f':
    	          	     nError=SendDoubleToSocket(nSocket,(* pQuery->data[nLoop].pDouble));
    	          	     break;
    	          }
    	          break;
    	}
    	if(nError) return 1;
        nLoop++;
     }//end while
     return 0;
}

int FetchStruction(long nCursorId,QUERYUNIT * que)
{
  long nRetVal;
  long col;
  long nColNum;
  long nType;
  int nOciSocket;
  char * sFormat;
  char sTemp[STRINGSIZE];
  
  nOciSocket=ReturnSocket();
  if(nOciSocket==0)  return -1012;

  sFormat=que->sFormat;
  if(SendLongToSocket(nOciSocket,(long)TASK_FETCHCURSOR))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(SendLongToSocket(nOciSocket,nCursorId))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(nRetVal==1)
  {
     if(GetLongFromSocket(nOciSocket,&nColNum))
     {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
     }
     for (col = 0; col < nColNum; col++)
     {
        sFormat=strchr(sFormat,'%');
        sFormat++;
        if(GetLongFromSocket(nOciSocket,&nType))
        {
             nOciSocket=0;
             return -ERR_NO_TCPIP;
        }
        switch (nType)
        {
             case TYPE_DOUBLE:
                  if(GetDoubleFromSocket(nOciSocket,que->data[col].pDouble))
                  {
                      return -ERR_NO_TCPIP;
                  }
                  break;
             case TYPE_LONG:
                  if(GetLongFromSocket(nOciSocket,que->data[col].pLong))
                  {
                      return -ERR_NO_TCPIP;
                  }
                  break;
             default:
                  if(GetStringFromSocket(nOciSocket,sTemp))
                  { 
                      return -ERR_NO_TCPIP;
                  }
                  if(!strncmp(sFormat,"ld",2)) *(que->data[col].pLong)=atol(sTemp);
                    else if(!strncmp(sFormat,"s",1))  strcpy(que->data[col].pChar,sTemp);
                       else if(!strncmp(sFormat,"lf",2)) *(que->data[col].pDouble)=atof(sTemp);
                  break;    	
        }          
      }//end for
    }//end if
    return nRetVal;
}

void ReleaseQueryUnit(QUERYUNIT * query)
{
    int nLoop=0;
    
    if(query==NULL) return;
    
    while(query->data[nLoop].pLong)
    {
    	free((void*)query->data[nLoop].pLong);
    	if((nLoop++)>ITEMSIZE) return;
    }
}
