#define DEBUG
/***********************************************************************
**filename	�ļ���:tasktool.c
**target	Ŀ��:���񹤾߳���ʵ���ļ�
**create time	��������: 2001.2.21
**original programmer	����Ա:�²�[chenbo]
**edit  history	�޸���ʷ:
**date     editer    reson
** 
****************************************************************************/
#include"tasktool.h"
#include"newtask.h"
#include"filechar.h"
#include"netmain.h"
#include"task.h"
#include"dlpublic.h"
#include<stdarg.h>
#include<stdlib.h>

extern char sTcpipErrInfo[81];
#ifdef HOUTAI
char sSysDlywService[21]="dlyw";
#else
extern char sSysDlywService[21];
#endif
long FindRoute(char * sDestination,char * sGateway);

#ifdef HOUTAI
static int  nClientSocket;
/*----------------------------------------------------------------
������ApplyToClient
Ŀ�ģ��Զ�װ�úͷ��ͷ��ظ��ͻ��˵�����
������soutstr:�����������������ַ���
      ...:Ҫ���͵�ʵ�ʲ���

˵����%s,%lf,%ld�ֱ�����ַ�����˫���ȣ�������
      %m:��ʾ����һ������������������ʵ��Ӧʹ��MakeMemory(���ȣ���������ַ)��
      %r:��������һ��RECORD�͵����ݣ�
������
      RECORD record;
      char * pointer;
      
      nval=ApplyToClient("%s%ld%lf%m%r","abc",100,19.30,
                                        MakeMemory(100,pointer),record);
                                        
      DropRecord(&record);
      return nval;
      

*---------------------------------------------------------------*/
long ApplyToClient(char * soutstr,...)
{

    TASKSTRUCT ans;
    MEMORY mem;
    RECORD rec;
    va_list pvar;
    char * pstr;
    long nposd=0,nposs=0,nposl=0;     
       
    memset(&ans,0,sizeof(TASKSTRUCT));
    
    
    ans.nRetVal=1;
    pstr=soutstr;
    va_start(pvar,soutstr);
    
    /////////////////////   ������ͳһװ��ģ��  ///////////////////////
    
    while(pstr[0])
    {
    	
        if(nposd>=DOUBLE_COUNT||nposl>=LONG_COUNT||nposs>=STRING_COUNT)
        {
        	strcpy(sTcpipErrInfo,"ĳ�෢�Ͳ����ĸ������ڹ涨�ĸ���!");
                va_end(pvar);
        	return -6;
        }	
    	if(pstr[0]!='%')
    	{
    	        pstr++;
    	        continue;
        }
    	pstr++;
    	switch(pstr[0])
    	{
    	    case 'l':
    	        pstr++;
    	        switch(pstr[0])
    	        {
    	             case 'f':
    	                   ans.dValue[nposd]=va_arg(pvar,double);
    	                   nposd++;
    	                   break;
    	             case 'd':
    	                   ans.nValue[nposl]=va_arg(pvar,long);
    	                   nposl++;
    	                   break;
    	             default:
            	           sprintf(sTcpipErrInfo,"\n�޷��������������ַ�����%s",soutstr);
    	                   va_end(pvar);
    	                   return -1;
    	        }           
    	        break;
            
            case 'r':
                rec=va_arg(pvar,RECORD);
                if(!rec.fp)
                {
                    ans.sValue[nposs]=rec.sdata;
                    ans.nLength[nposs]=strlen(rec.sdata)+1;
                }
                else
                {
                    ans.sValue[nposs]=rec.filename;
                    ans.nLength[nposs]=-rec.nsize;
                }        
                nposs++;
                break;
  
            case 's':
                ans.sValue[nposs]=va_arg(pvar,char *);
                ans.nLength[nposs]=strlen((char*)ans.sValue[nposs])+1;
                nposs++;
                break;
            
            case 'm':
                mem=va_arg(pvar,MEMORY);
                ans.sValue[nposs]=mem.pointer;
                ans.nLength[nposs]=mem.nlen;
                nposs++;
                break;
                                 
	    default:
   	        sprintf(sTcpipErrInfo,"\n�޷��������������ַ�����%s",soutstr);
    	        va_end(pvar);
    	        return -1;
                    	                   
    	}//end switch
    	
    }//end while 		     
    return SendTaskStructToSocket(nClientSocket,&ans); 
}

long dsTaskAutoRunTask(int nInSocket,long nTaskNo)
{

     TASKSTRUCT  req,ans;
     char sgateway[21];
     int nOutSocket,nerror;
     
     memset(&req,0,sizeof(TASKSTRUCT));
     memset(&ans,0,sizeof(TASKSTRUCT));
     memset(sgateway,'\0',sizeof(sgateway));
     
     ///////////////   ���տͻ���������������ṹ  //////////////////
     nClientSocket=nInSocket;

     if(GetTaskStructFromSocket(nInSocket,&req))
     {
     	DestroyTaskStruct(&req);
      	return -1;
     }

     //////////////////////        ·��ѡ��       //////////////////////
     if(!FindRoute(req.sDestination,sgateway))
     {
        
        DestroyTaskStruct(&req);
        ans.nRetVal=-1;
        SendTaskStructToSocket(nInSocket,&ans);
        return -1;
     }
     ///////////////////////   ������Դ������ת����  ///////////////////       
     if(strcmp(sgateway,"localhost"))
     {
        req.nRouteNum--;
     
        if((nOutSocket=ConnectRemote(sgateway,sSysDlywService))<0||req.nRouteNum<1)
        {
            DestroyTaskStruct(&req);
            ans.nRetVal=-2-(req.nRouteNum<1);
            SendTaskStructToSocket(nInSocket,&ans);
            return -1;
        }
     
        nerror=SendLongToSocket(nOutSocket,nTaskNo);
        if(!nerror) nerror=SendTaskStructToSocket(nOutSocket,&req);
     
        DestroyTaskStruct(&req);
        if(nerror)
        {
            ans.nRetVal=-4;
            SendTaskStructToSocket(nInSocket,&ans);
            close(nOutSocket);
            return -1;
        }
            
        if(GetTaskStructFromSocket(nOutSocket,&ans))
        {
            ans.nRetVal=-5;
            SendTaskStructToSocket(nInSocket,&ans);
            close(nOutSocket);
            return -1;
        }
        DisconnectRemote(nOutSocket);
        SendTaskStructToSocket(nInSocket,&ans);
        DestroyTaskStruct(&ans);
        return 0;
     }
     ///////////////////      ����Դ����    //////////////////               

     if(DispatchTask(&req))
     {
        ans.nRetVal=-6;
        DestroyTaskStruct(&req);
        SendTaskStructToSocket(nInSocket,&ans);
        close(nOutSocket);
        return -1;
     } 
     DestroyTaskStruct(&req);
     return 0;
}

#endif

MEMORY MakeMemory(long nlen,char * pointer)
{

    MEMORY mem;
    mem.nlen=nlen;
    mem.pointer=pointer;
    return mem;

}


void DestroyTaskStruct(TASKSTRUCT * task)
{
    long ndel=0;
    
    while(ndel<STRING_COUNT)
    {
        if(task->nLength[ndel]<0)
        	remove(task->sValue[ndel]);

    	if(task->nLength[ndel])
    		free(task->sValue[ndel]);
    	ndel++;
    }	

}

long GetTaskStructFromSocket(int nsock,TASKSTRUCT * task)
{
    RECORD rec;
    char sbuffer[INCREMENT_SIZE],sdata[4096];
    FILE * fp;
    long nrec,nfilesize,nLength,nRetVal;
    int nlen;
    
    
    nlen=sizeof(TASKSTRUCT);

    rec.fp=NULL;
    rec.filename=NULL;
    rec.sdata=sdata;
    rec.scurrent=sdata;

    if(GetStringFromSocket(nsock,rec.sdata)) return -1;
    rec.nsize=strlen(rec.sdata);
    

    ExportRecord(&rec,"%ld%s%ld%ld",&task->nTaskNo,task->sDestination,
                                   &task->nRouteNum,&task->nRetVal);
    ExportRecord(&rec,"%ld[30]",task->nValue);
    ExportRecord(&rec,"%lf[30]",task->dValue);
    ExportRecord(&rec,"%ld[20]",task->nLength);


    nrec=0;
    if(task->nRetVal>0)
    {
        while(task->nLength[nrec]!=0&&nrec<STRING_COUNT)
        {
            nlen=(int)task->nLength[nrec];
            if(nlen>0)
            {
                  if(!(task->sValue[nrec]=(char*)malloc(nlen)))
                  {
            	      DestroyTaskStruct(task);
            	      task->nRetVal=-5;
            	      return -2;
                  }	
                  if(GetStringFromSocket(nsock,(char*)task->sValue[nrec]))
                  {
            	      DestroyTaskStruct(task);
            	      task->nRetVal=-5;
            	      return -2;
                  }	
      	    }
      	    else
      	    {
                  if(!(task->sValue[nrec]=(char*)malloc(15)))
                  {
            	      DestroyTaskStruct(task);
            	      task->nRetVal=-5;
            	      return -2;
                  }
                
                  if(GetTempFileName((char*)task->sValue[nrec])<0)
                  {
            	      DestroyTaskStruct(task);
            	      task->nRetVal=-5;
            	      return -2;
                  }
                  
                  if(!(fp=fopen(task->sValue[nrec],"wb")))
                  {
            	      DestroyTaskStruct(task);
            	      task->nRetVal=-5;
            	      return -2;
                  }
                  
                  nfilesize=0;
                  for(;;)
                  {
                      memset(sbuffer,'\0',INCREMENT_SIZE);
                      GetLongFromSocket(nsock,&nLength);
                      if(GetOneRecordFromSocket(nsock,sbuffer,
                               (int)nLength)<nLength)
                      {
                  	     DestroyTaskStruct(task);
                	     task->nRetVal=-5;
            	             return -2;
                      }
                      nfilesize+=nLength;
                      fwrite(sbuffer,nLength,1,fp);
                      if(nfilesize>=(-task->nLength[nrec]))  break;
                      
                  } //end for   	     
      	          fclose(fp);
      	    }//end else
      	    nrec++;
        }//end while
    
    }//end if
    return 0;

}


long SendTaskStructToSocket(int nsock,TASKSTRUCT * task)
{
    RECORD rec;
    long nrec,nfilesize,nRet,nLength;	
    int nlen,nRetVal;
    char sbuffer[INCREMENT_SIZE];
    FILE * fp;    

    rec=CreateRecord();
    if(rec.nsize<=0) return -1;
    ImportRecord(&rec,"%ld%s%ld%ld",task->nTaskNo,task->sDestination,
                                     task->nRouteNum,task->nRetVal);
    ImportRecord(&rec,"%ld[30]",task->nValue);
    ImportRecord(&rec,"%lf[30]",task->dValue);
    ImportRecord(&rec,"%ld[20]",task->nLength);

    if(SendStringToSocket(nsock,rec.sdata))
    {
         DropRecord(&rec);
         return -2;
    }
    DropRecord(&rec);

    nrec=0;
    if(task->nRetVal>0)
    {
        while(task->nLength[nrec]!=0&&nrec<STRING_COUNT)
        {
               nlen=(int)task->nLength[nrec];
               if(nlen>0)
               {
                   if(SendStringToSocket(nsock,
                      (char *)task->sValue[nrec])) return -2;
               }
               else
               {
                  nfilesize=0;
                  if(!(fp=fopen(task->sValue[nrec],"rb"))) return -3;
                  for(;;)
                  {
                      memset(sbuffer,'\0',sizeof(sbuffer));
                      nlen=(-task->nLength[nrec])-nfilesize; 
                      if(nlen>(INCREMENT_SIZE-1)) nlen=INCREMENT_SIZE-1;
                      fread(sbuffer,nlen,1,fp);
                      nfilesize+=nlen;

                      SendLongToSocket(nsock,(long)nlen); 
                      if(SendRecordToSocket(nsock,sbuffer,nlen))
                      {
            	           fclose(fp);  
            	           return -2;
                      }
                      if(nfilesize>=(-task->nLength[nrec])) break;
                  } //end for   	     
      	          fclose(fp);
      	       }//end else
      	       nrec++;
        }//end while	
    }
    return 0;
}

/*---------------------------------------------------------------*
������CreateRecord
Ŀ�ģ�����һ����¼������
���أ���¼������ʵ��
*----------------------------------------------------------------*/

RECORD CreateRecord()
{

    RECORD rec;
    memset(&rec,0,sizeof(RECORD));
    
    if(rec.sdata=(char*)malloc(INCREMENT_SIZE))
    {
         rec.filename=NULL;
         rec.fp=NULL;
         rec.scurrent=rec.sdata;
         rec.nsize=INCREMENT_SIZE;
    }     
    return rec;
}

/*---------------------------------------------------------------*
������ImportRecord
Ŀ�ģ����¼�����������Ӽ�¼
������rec:�Ѿ������õļ�¼�����ݵ�ָ��
      arglist:�����������������
      ...:ʵ�ʲ�������
���أ���¼������ʵ��
���У�
      AppendRecord(rec,"%s%lf%ld","abc",11.23,500);
*----------------------------------------------------------------*/

long ImportRecord(RECORD * rec,char * arglist,...)
{
    
    char sbuffer[INCREMENT_SIZE];
    char * pointer;
    char * list;
    long ncount,ntype,address,temp;
    va_list pvar;
    
    
    memset(sbuffer,0,sizeof(sbuffer));
    pointer=sbuffer;
    list=arglist;
    
    va_start(pvar,arglist);
    while(list[0])
    {
    	if(list++[0]!='%') continue;
        ntype=GetArgType(list);
        ncount=GetArgCount(list);
        
        if(ncount<0)  address=va_arg(pvar,long);
        
        switch(ntype)
        {
             case TYPE_LONG:
              	  for(temp=0;temp<abs(ncount);temp++)
              	  {
              	  	if(ncount>0)
              	  	     sprintf(pointer,"%ld|",va_arg(pvar,long));
              	  	else
              	  	{
              	  	     sprintf(pointer,"%ld|",*((long*)address));
              	  	     address+=sizeof(long);
              	  	}     
              	  	while(pointer[0]) pointer++;     
              	  }
              	  break;  
        	   
             case TYPE_DOUBLE:
              	  for(temp=0;temp<abs(ncount);temp++)
              	  {
              	  	if(ncount>0)
              	  	     sprintf(pointer,"%.2lf|",va_arg(pvar,double));
              	  	else
              	  	{
              	  	     sprintf(pointer,"%.2lf|",*((double*)address));
              	  	     address+=sizeof(double);
              	  	}     
              	  	while(pointer[0]) pointer++;     
              	  }
              	  break;  

             case TYPE_STRING:
              	  for(temp=0;temp<abs(ncount);temp++)
              	  {
              	  	if(ncount>0)
              	  	     sprintf(pointer,"%s|",va_arg(pvar,char *));
              	  	else
              	  	{
              	  	     sprintf(pointer,"%s|",address);
              	  	     address+=sizeof(char *);
              	  	}     
              	  	while(pointer[0]) pointer++;     
              	  }
              	  break;  
        }//end switch
    	    
    }//end while	  
    va_end(pvar);

    ncount=strlen(sbuffer);
    
    if(rec->fp==NULL)
    {
       if((rec->nsize-(rec->scurrent-rec->sdata))<(ncount+2))
       {
           if(!(rec->filename=(char *)malloc(20))) return -1;
           if(GetTempFileName(rec->filename)<0) return -1;
           if(!(rec->fp=fopen(rec->filename,"w+"))) return -1;
           rec->nsize=strlen(rec->sdata)+ncount;
           fwrite(rec->sdata,strlen(rec->sdata),1,rec->fp);
           fwrite(sbuffer,ncount,1,rec->fp);
           fflush(rec->fp);
           realloc(rec->sdata,1);    
       }
       else 
       {
       	    strcpy(rec->scurrent,sbuffer);
            while(rec->scurrent[0]) rec->scurrent++;
       }
    }
    else
    {
       rec->nsize+=ncount;
       fwrite(sbuffer,ncount,1,rec->fp);
       fflush(rec->fp);
    }
    return 1;

}


static long GetArgType(char * list)
{
     
     if(list[0]=='l'&&list[1]=='d') return TYPE_LONG;
     if(list[0]=='l'&&list[1]=='f') return TYPE_DOUBLE;
     if(list[0]=='s') return TYPE_STRING;
     return 0;
}

static long GetArgCount(char * list)
{

     long ncount=0;
     long nsign=0;
     
     while(list[0]&&list[0]!='%'&&list[0]!='['&&list[0]!='(')  list++;
     if(list[0]=='(') nsign=1;
     else if(list[0]=='[') nsign=-1;
     else return 1;
     
     list++;
     while(list[0]>='0'&&list[0]<='9')
     {
     	 
     	 ncount=ncount*10+(list[0]-'0');
     	 list++;
     }
     return nsign*ncount;	 

}


/*---------------------------------------------------------------*
������RewindRecord
Ŀ�ģ���¼ָ�븴λ
���أ�������ɹ�
*---------------------------------------------------------------*/
long RewindRecord(RECORD * rec)
{
     if(rec==0) return;
     rec->scurrent=rec->sdata;
     if(rec->fp&&rec->filename) rewind(rec->fp);
     
     
}

/*---------------------------------------------------------------*
������ExportRecord
Ŀ�ģ��Ӽ�¼�������е�������
������rec:�Ѿ������õļ�¼�����ݵ�ָ��
      arglist:������������������
      ...:ʵ�ʲ�������
���أ�����0��ʾ���м�¼������0��ʾ��¼�Ѿ�ȡ�꣬С��0��ʾ����
���У�
      ExportRecord(rec,"%s%lf%ld",sValue,&dValue,&nValue);
*----------------------------------------------------------------*/

long ExportRecord(RECORD * rec,char * arglist,...)
{
    va_list pvar;
    long ntype,ncount,address,temp;
    char * list;
    

    if(rec==NULL) return -1;
    if((!rec->fp)&&((!rec->scurrent)||rec->scurrent[0]=='\0')) return 0;
    list=arglist;
    va_start(pvar,arglist);         	
    while(list[0])
    {
    	if(list++[0]!='%') continue;
        ntype=GetArgType(list);
        ncount=GetArgCount(list);
        if(ncount<0)  address=va_arg(pvar,long);


        switch(ntype)
        {
             case TYPE_LONG:
              	  for(temp=0;temp<abs(ncount);temp++)
              	  {
              	  	if(ncount>0)
              	  	{
              	  	     if(rec->fp)
              	  	     {
              	  	     	  if(!FetchFile(rec->fp,"%d|",va_arg(pvar,long *))) return 0;
              	  	     }
                             else
                             {                	  	     	
                	  	  rec->scurrent=FetchStringToArg(rec->scurrent,"%d|",va_arg(pvar,long *));
                	     }	  
                        }
              	  	else
              	  	{
              	  	     if(rec->fp)
              	  	     {
              	  	           if(!FetchFile(rec->fp,"%d|",(long*)address)) return 0;
              	  	     }      
                             else                	  	     	
                             {
              	  	          rec->scurrent=FetchStringToArg(rec->scurrent,"%d|",(long*)address);
              	  	     }     
              	  	     address+=sizeof(long);
              	  	}     
              	  }
              	  break;  
        	   
             case TYPE_DOUBLE:
              	  for(temp=0;temp<abs(ncount);temp++)
              	  {
              	  	if(ncount>0)
              	  	{
              	  	     if(rec->fp)
              	  	     {
              	  	     	  if(!FetchFile(rec->fp,"%f|",va_arg(pvar,double *))) return 0;
              	  	     }
                             else
                             {                	  	     	
               	  	          rec->scurrent=FetchStringToArg(rec->scurrent,"%f|",va_arg(pvar,double *));
               	  	     }     
              	  	}
              	  	else
              	  	{
              	  	     if(rec->fp&&rec->filename)
              	  	     {
              	  	     	  if(!FetchFile(rec->fp,"%f|",(double*)address)) return 0;
              	  	          
              	  	     }
                             else                	  	     	
                             {
              	  	          rec->scurrent=FetchStringToArg(rec->scurrent,"%f|",(double*)address);
              	  	     } 
              	  	     address+=sizeof(double);
              	  	}     
              	  }
              	  break;  

             case TYPE_STRING:
             	  for(temp=0;temp<abs(ncount);temp++)
             	  {
             	  	
              	        if(ncount>0)
              	        {
              	  	     if(rec->fp)
              	  	     {
              	  	     	  FetchFile(rec->fp,"%s|",va_arg(pvar,char *));
                 	     }
                             else
                             {                	  	     	
                	  	  rec->scurrent=FetchStringToArg(rec->scurrent,"%s|",va_arg(pvar,char *));
                	     }
                        }	     
             	        else
              	        {
              	  	     if(rec->fp&&rec->filename)
              	  	     {
              	  	     	  if(!FetchFile(rec->fp,"%s|",
              	  	     	     (char *)address)) return 0;
              	  	          
              	  	     }
                             else                	  	     	
                             {
              	  	          rec->scurrent=FetchStringToArg(rec->scurrent,"%s|",
              	  	                        (char*)address);
              	  	     } 
              	  	     address+=sizeof(char *);
               	        }	     
              	  }//end for     
              	  break;  
        }//end switch
    	    
    }//end while	  
    va_end(pvar);
    return 1; 
}
/*---------------------------------------------------------------*
������DropRecord
Ŀ�ģ��ͷż�¼��ʹ�õĿռ�
������rec����¼�����ݵ�ָ��
ע�⣺�ڼ�¼ʹ����Ϻ����ʹ�ñ���������������ڴ������˷�
*---------------------------------------------------------------*/

void DropRecord(RECORD * rec)
{
     if(rec==0) return;

     if(rec->sdata) free(rec->sdata);
     rec->sdata=NULL;
     rec->nsize=0;
     if(rec->fp)
     {
     	fclose(rec->fp);
        rec->fp=NULL;
     }     
     if(rec->filename)
     {
     	remove(rec->filename);
     	free(rec->filename);
     }	

}


/*----------------------------------------------------------------
������AutoRunTask
Ŀ�ģ��ͻ��˷����̨�������
������sdest:����Ŀ�ĵص�ַ��
      ntask:�����
      sinstr:����������������ַ���
      soutstr:����������������ַ���
      ...:���������������
      
˵�����������ڿͻ��˵��÷���RunTaskһ�£������������������ͣ�
      %m%r:�ֱ����������ڴ�����ͼ�¼�����ݣ���ʹ�÷�����ο�
      ApplyToClient������
*----------------------------------------------------------------*/

long AutoRunTask(char * sdest,long ntask,char * sinstr,char * soutstr,...)
{

    TASKSTRUCT req,ans;
    MEMORY      mem;
    RECORD      rec,*prec;
    
    char * pstr;
    int nsock;
    long nposl=0,nposd=0,nposs=0;
    char sgateway[21];
    va_list  pvar;
    

    memset(&req,0,sizeof(TASKSTRUCT));
    memset(&ans,0,sizeof(TASKSTRUCT));
    req.nTaskNo=ntask;
    strcpy(req.sDestination,sdest);
    req.nRouteNum=MAX_ROUTE_NUM;
    req.nRetVal=1;
    pstr=sinstr;
    va_start(pvar,soutstr);
    
    ///////////////////   ������ͳһװ��ģ��  ///////////////////////
    
    while(pstr[0])
    {
    	
        if(nposd>=DOUBLE_COUNT||nposl>=LONG_COUNT||nposs>=STRING_COUNT)
        {
        	strcpy(sTcpipErrInfo,"ĳ�෢�Ͳ����ĸ������ڹ涨�ĸ���!");
                va_end(pvar);
        	return -6;
        }	
    	if(pstr[0]!='%')
    	{
    	        pstr++;
    	        continue;
    	}
    	pstr++;
    	switch(pstr[0])
    	{
    	    case 'l':
    	        pstr++;
    	        switch(pstr[0])
    	        {
    	             case 'f':
    	                   req.dValue[nposd]=va_arg(pvar,double);
    	                   nposd++;
    	                   break;
    	             case 'd':
    	                   req.nValue[nposl]=va_arg(pvar,long);
    	                   nposl++;
    	                   break;
    	             default:
            	           sprintf(sTcpipErrInfo,"\n�޷���������������ַ�����%s",sinstr);
    	                   va_end(pvar);
    	                   return -1;
    	        }           
    	        break;
            
            case 's':
                req.sValue[nposs]=va_arg(pvar,char *);
                req.nLength[nposs]=strlen((char*)req.sValue[nposs])+1;
                nposs++;
                break;

            case 'r':
                rec=va_arg(pvar,RECORD);
                if(!rec.fp)
                {
                     req.sValue[nposs]=rec.sdata;
                     req.nLength[nposs]=strlen((char*)req.sValue[nposs])+1;
                }
                else
                {
                     req.sValue[nposs]=rec.filename;
                     req.nLength[nposs]=-rec.nsize;
                }
                nposs++;
                break;
            
            case 'm':
                mem=va_arg(pvar,MEMORY);
                req.sValue[nposs]=mem.pointer;
                req.nLength[nposs]=mem.nlen;
                nposs++;
                break;
                                 
	    default:
                sprintf(sTcpipErrInfo,"\n�޷���������������ַ�����%s",sinstr);
    	        va_end(pvar);
    	        return -1;
                    	                   
    	}//end switch
    	
    }//end while 		     

    //////////////   ·��ѡ��������ͨѶģ��         //////////////////// 
    
    if(!FindRoute(req.sDestination,sgateway))
    {
    	sprintf(sTcpipErrInfo,"�Ҳ���Ŀ�ĵ�%s��·��..",req.sDestination);
    	return -2;
    }
    	    
    if((nsock=ConnectRemote(sgateway,sSysDlywService))<0)
    {
    	sprintf(sTcpipErrInfo,"�޷���Զ������%s��dlyw����������..",sgateway);
    	return -3;
    }    	
    
    if(SendLongToSocket(nsock,TASK_AUTORUNTASK))
    {
    	sprintf(sTcpipErrInfo,"��Զ������%s�������������������..",sgateway);
        close(nsock);
    	return -4;
    }

    if(SendTaskStructToSocket(nsock,&req))
    {
    	sprintf(sTcpipErrInfo,"��Զ������%s�������������������..",sgateway);
        close(nsock);
    	return -4;
    
    }
 
    
    ////////////////    �������Ͳ���      /////////////////////////
    if(GetTaskStructFromSocket(nsock,&ans))
    {
    	strcpy(sTcpipErrInfo,"�������񷵻ز�������..");
        close(nsock);
    	return -5;
    }

    if(ans.nRetVal<0)
    {
    	switch(ans.nRetVal)
    	{
              case -1:
                  strcpy(sTcpipErrInfo,"�����Ҳ���Ŀ�ĵص�·�ɣ�");
                  break;    	    
    		    
              case -2:
                  strcpy(sTcpipErrInfo,"�����޷���Զ��������������");
                  break;

              case -3:
                  strcpy(sTcpipErrInfo,"������·���������涨ֵ");
                  break;

              case -4:
                  strcpy(sTcpipErrInfo,"������תʧ��");
                  break;    	    

              case -5:
                  strcpy(sTcpipErrInfo,"�����ϼ����ؽ��ʧ��");
                  break;    	    
                      	    
              case -6:
                  strcpy(sTcpipErrInfo,"����Դ����ַ�ʧ��");
                  break;
        }
        close(nsock);
        return ans.nRetVal-10;
    }//end if                	    
                      	    
    
    ///////////////////   ������ز���    ////////////////////////
    nposl=0;
    nposd=0;
    nposs=0;
    pstr=soutstr;
    
    while(pstr[0])
    {
    	if(pstr[0]!='%')
    	{
    	        pstr++;
    	        continue;
    	}
    	pstr++;
    	switch(pstr[0])
    	{
    	    case 'l':
    	        pstr++;
    	        switch(pstr[0])
    	        {
    	             case 'f':
    	                   * ((double *)va_arg(pvar,double *))=ans.dValue[nposd];
    	                   nposd++;
    	                   break;
    	             case 'd':
    	                   * ((long *)va_arg(pvar,long * ))=ans.nValue[nposl];
    	                   nposl++;
    	                   break;
    	             default:
            	           sprintf(sTcpipErrInfo,"�޷��������������ַ�����%s",soutstr);
            	           DestroyTaskStruct(&ans);
    	                   va_end(pvar);
                           close(nsock);
    	                   return -1;
    	        }           
    	        break;
            
            case 'r':
                prec=va_arg(pvar,RECORD *);
                if(ans.nLength[nposs]>0)
                {
                   prec->fp=NULL;
                   prec->filename=NULL;
                   prec->sdata=ans.sValue[nposs];
                   prec->nsize=ans.nLength[nposs];
                   prec->scurrent=prec->sdata;
                }
                else
                {
                   prec->sdata=NULL;
                   prec->scurrent=NULL;
                   prec->nsize=-ans.nLength[nposs];
                   prec->filename=ans.sValue[nposs];
                   prec->fp=fopen(prec->filename,"rt");
                   ans.nLength[nposs]=0;
                }   
                ans.sValue[nposs]=0;
                ans.nLength[nposs]=0;
                nposs++;
                break;
                
            case 's':
            case 'm':
                memcpy(va_arg(pvar,char *),ans.sValue[nposs],ans.nLength[nposs]);
                nposs++;
                break;
 	    
 	    default:
                sprintf(sTcpipErrInfo,"�޷��������������ַ�����%s",soutstr);
                DestroyTaskStruct(&ans);
    	        va_end(pvar);
                close(nsock);
    	        return -1;
                    	                   
    	}//end switch
    	
    }//end while 		     
    va_end(pvar);
    DestroyTaskStruct(&ans);
    DisconnectRemote(nsock);
    return 0;
    
}

long GetRecordSize(RECORD * rec)
{
     if(rec==NULL) return 0;
     if(rec->fp)  return rec->nsize;
     return strlen(rec->sdata);
}





long FindRoute(char * sDestination,char * sGateway)
{
     extern char sSysDlywHost[41];
     
     if(strcmp(sSysDlywHost,"localhost"))
     {
     	strcpy(sGateway,sSysDlywHost);
     	return 1;
     }	
     if(RunSelect("select lower(sgateway) from dl$route where \
        sdestination=%s into %s",sDestination,sGateway)>0) return 1;
        
     if(RunSelect("select lower(sgateway) from dl$route where \
        upper(sdestination)='DEFAULT' into %s",sGateway)>0) return 1;
        
     return 0;   

}
