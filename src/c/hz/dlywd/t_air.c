//民航代售票系统模块：省局，地区局
//Original programmer : chenbo

#include"tasktool.h"
#include"dlywd.h"

extern nSysLkrq,nSysSfm;
extern char sTcpipErrInfo[81];

//得到所有的城市名称及代码和取票方式
long dsTaskGetAllCity()
{

    RECORD rec,rec1;
    long nId;
    char buf[51];
    char sCity[41],sSearch[11],sCode[4],sQpdh[3],sQpmc[20];
    
    nId=OpenCursor("select cname,csearch,ccode from ticket.dl$air_city "
		   "order by -nweight");
    if(nId<0) return -1;

    rec=CreateRecord();    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%s%s",sCity,sSearch,sCode)<=0) break;
    	alltrim(sCity);
    	alltrim(sSearch);
    	alltrim(sCode);
    	sprintf(buf,"%s->%s[%s]",sSearch,sCity,sCode);
    	ImportRecord(&rec,"%s%s",buf,sSearch);
    }
    CloseCursor(nId);
    
    nId=OpenCursor("select cqpdh,cqpmc from ticket.dl$air_qpfs ");
    if(nId<0) return -1;

    rec1=CreateRecord();    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%s",sQpdh,sQpmc)<=0) break;
    	alltrim(sQpmc);
    	alltrim(sQpdh);
    	sprintf(buf,"%s->%s",sQpdh,sQpmc);
    	ImportRecord(&rec1,"%s%s",buf,sQpdh);
    }
    CloseCursor(nId);
    
    return ApplyToClient("%r%r",rec,rec1);
}

//航线查询
long dsTaskQueryAirLine(char * sJh,char * sFromCode,char * sDestCode,long nAirDate,long nAirTime,char * sCzy)
{
   
    char sBuffer[4096];
    char sResult[4096],sTest[4096];
    char inbuf[4096];
    char sAction[1024];
    char sParse[1024];
    char sTemp[4096];
    char sJym[16];
    char route[7];
    char * sMonth[12]={"JAN","FEB","MAR","APR","MAY","JUN",
                       "JUL","AUG","SEP","OCT","NOV","DEC"};
    char * offset;                       
    char * check;
    char * data;
    char * valid;
    char cCs[4]=".";
    long nMonth,nDay,nCount,copy,nLoop,nRetVal,nRet,nret;                   
    
    RunSelect("select ccs from dl$jymcsfb where nbh=9 into %s",cCs);
    alltrim(cCs);
    if(!strcmp(cCs,"y") || !strcmp(cCs,"Y"))
       return ApplyToClient("%s","000111|该业务因故停止，请与速递局联系...|");
    
    memset(sBuffer,'\0',sizeof(sBuffer));
    memset(sResult,'\0',sizeof(sResult));
    memset(sAction,'\0',sizeof(sAction));
    memset(sParse,'\0',sizeof(sParse));
    
    nMonth=((nAirDate%10000)/100);
    nDay=nAirDate%100;
    
    if(nAirTime==0)     
         sprintf(sAction,"AV:%s%s/%02ld%s\n",
    				     sFromCode,sDestCode,nDay,sMonth[nMonth-1]);
    else                  
         sprintf(sAction,"AV:%s%s/%02ld%s/%04ld\n",
                 		  sFromCode,sDestCode,nDay,sMonth[nMonth-1],nAirTime);
    
    nRetVal=dsAirSendAndRecv(-1,sAction,sBuffer);
    if(nRetVal)
    {
        GetAirErrInfo(nRetVal);
        sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
    	return ApplyToClient("%s",sResult);
    }
    sprintf(sAction,"%s%s",sFromCode,sDestCode);
    offset=FetchStringToArg(sBuffer,"%s\n",sParse);
    if(!offset)
    	return ApplyToClient("%s","000001|接收到的数据格式错误!|");
    
    if(!strstr(sParse,sAction))
    {
    	sprintf(sAction,"000001|查询错误：%s|",sBuffer);
    	return ApplyToClient("%s",sAction);
    }
    check=offset;
    nCount=0;

    for(;;)
    {
        data=check;
    	if(strlen(check)<50) { nCount--;  break; }
    	check=FetchStringToArg(check,"%s\n",sParse);
    	if(!check && nCount==0)
    	{
       	    sprintf(sAction,"000001|查询错误：%s|",sBuffer);
    	    return ApplyToClient("%s",sAction);
    	}
    	if(!check && nCount>0) break;    
    	valid=sParse;
//    	while(valid[0]==' ') valid++;
    	if(valid[0]<'0' || valid[0]>'9') continue;
    	nCount++;
    }

    if(nCount==0)
    	return ApplyToClient("%s","000002|没有相应航班的信息!|");
   
    nret=nCount;
    check=offset;
    data=sResult;
    sprintf(data,"000000|%ld|",nCount);
    while(data[0]) data++;

    for(;nCount>0;nCount--)
    {
           	
    	check=FetchStringToArg(check,"%s\n",sParse);
    	valid=sParse;
        if(strlen(valid)>80) break;
    	//filed 1 : 序号  跳过
//    	while(valid[0]==' ') valid++;
    	if(valid[0]<'0' || valid[0]>'9') continue;
    	while(valid[0]!=' ') valid++;
    	while(valid[0]==' ') valid++;
    	
    	//field 2 : 航班号
    	while(valid[0]!=' ')
    	{
	    data[0]=valid[0];

    	    data++;
    	    valid++;
    	}

    	data[0]='|';
    	data++;
    	    
    	//field 3 : 城市对 
    	while(valid[0]==' ') valid++;
    	while(valid[0]!=' ')
    	{
    	    data[0]=valid[0]; 
    	    valid++;
    	    data++;
        }
    	data[0]='|';
    	data++;
            	    
        //field 4  : 起飞时间    	     
    	while(valid[0]==' ') valid++;
    	memcpy(data,valid,4);
    	data+=4;
    	valid+=4;
    	data[0]='|';
    	data++;

        //field 5  : 降落时间     	
    	while(valid[0]==' ') valid++;
    	memcpy(data,valid,4);
    	data+=4;
    	valid+=4;
    	data[0]='|';
    	data++;

	//field 6: 机型

    	while(valid[0]==' ') valid++;
    	while(valid[0]!=' ')
    	{
    	    data[0]=valid[0]; 
    	    valid++;
    	    data++;
        }
        data[0]='|';
        data++;

        //field 7:  经停点(0，直达)
        while(valid[0]==' ') valid++;
    	memcpy(data,valid,1);
    	data+=1;
    	valid+=1;
    	data[0]='|';
    	data++;

        //field 8,9 :跳过
    	/*while(valid[0]==' ') valid++;
    	memset(valid,' ',4);
    	while(valid[0]==' ') valid++;
    	valid+=2;*/
        valid+=12;
    	
    	//field 10 : 舱位信息
    	strcpy(data,valid);
    	while(data[0]) data++;
    	data[0]='|';
    	data++;
    	
    }//end for
 
    RunSelect("select ccs from dl$jymcsfb where nbh=11 into %s",cCs);
    if(!strcmp(cCs,"y") || !strcmp(cCs,"Y"))
    {
       strcpy(sTemp,sResult);
       memset(sTest,'\0',sizeof(sTest));
       nRet=dsPnQuery(sFromCode,sDestCode,sTest);
       if(nRet)
         sprintf(sResult,"000000|%ld|%s%s",(nRet+nret),(sTemp+9),sTest);
       //printf("\n[%s]\n",sResult);
    }//test 

    sprintf(route,"%s%s",sFromCode,sDestCode);
    strcpy(inbuf,sResult);
    //printf("\nRESULT=[%s]\n",sResult);
    InsertAirQuery(inbuf,nAirDate,nAirTime,sJh,sCzy,route);
    //printf("\nBUFER=[%s]",inbuf);
    
    return ApplyToClient("%s",inbuf);
    //return ApplyToClient("%s",sResult);
}

long dsPnQuery(char *sFromCode,char *sDestCode,char *sResult)
{
    char sBuffer[4096];
    char inbuf[4096];
    char sAction[1024];
    char sParse[1024];
    
    char * offset;                       
    char * check;
    char * data;
    char * valid;
    long nret,nCount,copy,nLoop,nRetVal;                   
    
    memset(sBuffer,'\0',sizeof(sBuffer));
    memset(sAction,'\0',sizeof(sAction));
    memset(sParse,'\0',sizeof(sParse));
    
    sprintf(sAction,"PN:\n");
    
    nRetVal=dsAirSendAndRecv(-1,sAction,sBuffer);
    if(nRetVal)
    {
        GetAirErrInfo(nRetVal);
        sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
    	return 0;
    }
    sprintf(sAction,"%s%s",sFromCode,sDestCode);
    offset=FetchStringToArg(sBuffer,"%s\n",sParse);
    if(!offset)
      return 0;
    
    if(!strstr(sParse,sAction))
       return 0;
    check=offset;
    nCount=0;
    for(;;)
    {
        data=check;
    	if(strlen(check)<50) break;
    	check=FetchStringToArg(check,"%s\n",sParse);
    	if(!check && nCount==0)
    	   return 0;
    	if(!check && nCount>0) break;    
    	valid=sParse;
//    	while(valid[0]==' ') valid++;
    	if(valid[0]<'0' || valid[0]>'9') continue;
    	nCount++;
    }
    if(nCount==0)
    	return 0;
   
    check=offset;
    data=sResult;
    nret=nCount;
    //sprintf(data,"000000|%ld|",nCount);
    //while(data[0]) data++;
    for(;nCount>0;nCount--)
    {
           	
    	check=FetchStringToArg(check,"%s\n",sParse);
    	valid=sParse;
    	//filed 1 : 序号  跳过
//    	while(valid[0]==' ') valid++;
    	if(valid[0]<'0' || valid[0]>'9') continue;
    	while(valid[0]!=' ') valid++;
    	while(valid[0]==' ') valid++;
    	
    	//field 2 : 航班号
    	while(valid[0]!=' ')
    	{
    	    data[0]=valid[0];
    	    data++;
    	    valid++;
    	}
    	data[0]='|';
    	data++;
    	    
    	//field 3 : 城市对 
    	while(valid[0]==' ') valid++;
    	while(valid[0]!=' ')
    	{
    	    data[0]=valid[0]; 
    	    valid++;
    	    data++;
        }
    	data[0]='|';
    	data++;
            	    
        //field 4  : 起飞时间    	     
    	while(valid[0]==' ') valid++;
    	memcpy(data,valid,4);
    	data+=4;
    	valid+=4;
    	data[0]='|';
    	data++;

        //field 5  : 降落时间     	
    	while(valid[0]==' ') valid++;
    	memcpy(data,valid,4);
    	data+=4;
    	valid+=4;
    	data[0]='|';
    	data++;

	//field 6: 机型

    	while(valid[0]==' ') valid++;
    	while(valid[0]!=' ')
    	{
    	    data[0]=valid[0]; 
    	    valid++;
    	    data++;
        }
        data[0]='|';
        data++;

       	while(valid[0]==' ') valid++;
        //field 7,8,9 :跳过
    	memset(valid,' ',4);
    	while(valid[0]==' ') valid++;
    	valid+=3;
    	
    	//field 10 : 舱位信息
    	strcpy(data,valid);
    	while(data[0]) data++;
    	data[0]='|';
    	data++;
    	
    }//end for
    return nret;
}

//航线入库
long InsertAirQuery(char * sBuffer,long nAirDate,long nAirTime,char * sJh,char * sCzy,char * route)
{
	char *offset,cCs[4];
	char sTemp[4096];
	char sAirLine[10],sAirType[20],sBerth[200],sRoute[20];
        long nStartTime,nEndTime,nJt=0; 
	long nCount,nRetVal,nId,nTimes=1,ncount=0;
	
    if(strncmp(sBuffer,"000000",6))  return 0;
    
    offset=sBuffer+7;
    nCount=0;
    offset=FetchStringToArg(offset,"%d|",&nCount);
    if(nCount<=0)
    {
    	printf("没有相应的航班信息...");
    	return 0;
    }

    alltrim(route);
    if(!strcmp(route,"CTUSIA")) strcpy(route,"CTUXIY");
    if(!strcmp(route,"SIACTU")) strcpy(route,"XIYCTU");
    nRetVal=RunSql("delete ticket.dl$air_query where \
		 croute=%s and ndata=%ld and ntime=%ld",
		 route,nAirDate,nAirTime);
    if(nRetVal>0) CommitWork(); //delete

    RunSelect("select ccs from dl$jymcsfb where nbh=10 into %s",cCs);
    alltrim(cCs);
    memset(sTemp,'\0',sizeof(sTemp));

    while(nCount--)
    {
        offset=FetchStringToArg(offset,"%s|%s|%d|%d|%s|%d|%s|",
              sAirLine,sRoute,&nStartTime,&nEndTime,sAirType,&nJt,sBerth);

	if(strcmp(route,sRoute)!=0) break;
	if(sBerth[0]=='\0') continue;
    	if(strlen(sAirLine)<5) continue;
        
        if(strchr(sBerth,'*') && (!strcmp(cCs,"Y") || !strcmp(cCs,"y")) && nJt==0 )
           	QueryAirBerth(sAirLine,nAirDate,sBerth);
        else
       		fillterberth(sBerth,sBerth);  //fillter some no use berth!
      
        if(strchr(sBerth,'\n')) FetchStringToArg(sBerth,"%s\n",sBerth);
        alltrim(sBerth);
        if(strlen(sBerth)<2) strcpy(sBerth,".");
        
        nRetVal=RunSql("INSERT INTO ticket.dl$air_query values(\
                                    %ld,%ld,%s,%s,%ld,%ld,%s,%s)",
                                    nAirDate,nAirTime,sAirLine,
                                    sRoute,nStartTime,nEndTime,
                                    sAirType,sBerth);
        if(nRetVal<0) break;
	ncount++;
        /*printf("\n(%ld)|%s|%s|%d|%d|%s|%ld|%s|",
             nCount,sAirLine,sRoute,nStartTime,nEndTime,sAirType,nJt,sBerth);*/

        sprintf(sTemp,"%s%s|%s|%ld|%ld|%s|%s|%ld|",
     	   	sTemp,sAirLine,sRoute,nStartTime,nEndTime,sAirType,sBerth,nJt);
    }

    if(nRetVal>0)
    {
		nId=RunSelect("select ntimes from ticket.dl$air_mana \
					where ndata=%ld and \
			 		cjh=%s and cczydh=%s into %ld",
					nSysLkrq,sJh,sCzy,&nTimes);

		if(nId>0)
		{
			nTimes++;
	  		RunSql("UPDATE ticket.dl$air_mana set ntimes=%ld \
					where ndata=%ld and cjh=%s and cczydh=%s",
					nTimes,nSysLkrq,sJh,sCzy);
		}
		else
		{
	   		nTimes=1;
	   		nId=RunSql("INSERT INTO ticket.dl$air_mana \
				values(%s,%ld,%ld,%s)",
				sJh,nSysLkrq,nTimes,sCzy);
			if(nId<0)
				return -1;
 		}
		CommitWork();
    }
    else RollbackWork();
    memset(sBuffer,'\0',sizeof(sBuffer));
    sprintf(sBuffer,"000000|%ld|%s",ncount,sTemp);
}

//舱位扩展处理！
void QueryAirBerth(char * sairline,long ndate,char * sberth)
{
    char sAction[1024]=".";
    char * offset;
    char sBerth[22]=".";
    long nRetVal,nMonth,nDay,i=0;
    char * sMonth[12]={"JAN","FEB","MAR","APR","MAY","JUN",
                       "JUL","AUG","SEP","OCT","NOV","DEC"};
   
    nMonth=((ndate%10000)/100);
    nDay=ndate%100;
    sprintf(sAction,"AV:%s/%02ld%s\n",sairline,nDay,sMonth[nMonth-1]);
    nRetVal=dsAirSendAndRecv(-1,sAction,sAction);   
    if(nRetVal)  return;

    //offset = sAction+34;
    offset = (strstr(sAction,"CTU"));
    offset +=4;
    offset[58]='\0';
    strcpy(sBerth,offset);
    //offset=FetchStringToArg(sAction,"%s\n",sBerth);
    fillterberth(sBerth,sBerth);  
    sberth[0]='\0';
    strcpy(sberth,sBerth);
    
}

//票价查询
long dsTaskQueryAirPrice(char * sCompany,char * sFromCode,char * sDestCode,long nAirDate)
{ 
    long nId,nRetVal;
    char sRoute[7];
    char cBerth[2];
    double dRebate,dPubbate,dBerbate;
    RECORD rec;
  
    sprintf(sRoute,"%s%s",sFromCode,sDestCode);
    alltrim(sRoute);
    rec=CreateRecord();

    if(RunSelect("select ROWNUM from ticket.dl$company_rebate "
    			 "where ccompany=%s",sCompany)<=0)
    {  
        ImportRecord(&rec,"%s%lf%lf%lf","1",0,0,0);
        return ApplyToClient("%r",rec);		
    }
    RunSelect("select dpubdate from ticket.dl$air_line where "
                  "cairline=%s into %lf ",sRoute,&dPubbate); 
    
    nId=OpenCursor("select distinct cberth from ticket.dl$company_rebate "
                           "where ccompany=%s",sCompany);
                                                      
    for(;;)
    {
       	if((nRetVal=FetchCursor(nId,"%s",cBerth))==0)
    	    break;
        if(RunSelect("select dberbate "
                   "from ticket.dl$company_rebate "
                   "where ccompany=%s AND "
                   "croute=%s AND "
                   "cberth=%s into %lf",
                   sCompany,sRoute,cBerth,&dBerbate)==0)        
           RunSelect("select dberbate "
                   "from ticket.dl$company_rebate "
                   "where ccompany=%s AND "
                   "croute='0' AND "
                   "cberth=%s  into %lf",
                   sCompany,cBerth,&dBerbate);
                   
        if(dBerbate==0) continue;
        dRebate=dPubbate*dBerbate/10;
   	    ROUND(&dRebate,-1);     
        
        ImportRecord(&rec,"%s%lf%lf%lf",
    	            cBerth,dPubbate,dRebate,dBerbate);			
    }
    CloseCursor(nId);
    return ApplyToClient("%r",rec);		
}

//经停点查询
long dsTaskQueryAirRoute(char * sAirLine,long nAirDate)
{
    char sResult[1024];
    char sBuffer[1024];
    char sAction[81];
    char sParse[1024];
    char * sMonth[12]={"JAN","FEB","MAR","APR","MAY","JUN",
                       "JUL","AUG","SEP","OCT","NOV","DEC"};
    char * offset;                       
    long nMonth,nDay,nRetVal;                   
     
    memset(sResult,'\0',sizeof(sResult));
    memset(sBuffer,'\0',sizeof(sBuffer));
    memset(sParse,'\0',sizeof(sParse));
    
    nMonth=((nAirDate%10000)/100);
    nDay=nAirDate%100;

    sprintf(sAction,"FF %s/%02ld%s\n",sAirLine,nDay,sMonth[nMonth-1]);
    nRetVal=dsAirSendAndRecv(-1,sAction,sBuffer);
    if(nRetVal)
    {
        GetAirErrInfo(nRetVal);
        sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
    	return ApplyToClient("%s",sResult);
    }
    
    offset = sResult;
    
    for(;;)
    {
    	offset = FetchStringToArg(offset,"%s\n",sParse);
    	if(!offset) break;
    	alltrim(sParse);
    	if(strlen(sParse)<3) break;
    	strcat(sBuffer,sParse);
    	strcat(sBuffer,"\n");
    }
    return ApplyToClient("%s",sBuffer);	
}

//扩展舱位查询
long dsTaskQueryAirBerth(char * sairline,long ndate)
{
    char sAction[1024]=".";
    char sAirLine[8],sFromCode[4];
    char * offset;
    char sBerth[22]=".";
    long nRetVal,nMonth,nDay,i=0;
    char * sMonth[12]={"JAN","FEB","MAR","APR","MAY","JUN",
                       "JUL","AUG","SEP","OCT","NOV","DEC"};
   
    FetchStringToArg(sairline,"%s|%s|",sAirLine,sFromCode);
    nMonth=((ndate%10000)/100);
    nDay=ndate%100;
    alltrim(sAirLine);
    alltrim(sFromCode);
    sprintf(sAction,"AV:%s/%02ld%s\n",sAirLine,nDay,sMonth[nMonth-1]);
    nRetVal=dsAirSendAndRecv(-1,sAction,sAction);   
    if(nRetVal)  return;

    //offset = sAction+34;
    offset = (strstr(sAction,sFromCode));
    offset +=4;
    offset[58]='\0';
    //printf("\n[%s]",offset);
    strcpy(sBerth,offset);
    
    fillterberth(sBerth,sBerth);  
    if(strchr(sBerth,'\n')) FetchStringToArg(sBerth,"%s\n",sBerth);
    alltrim(sBerth);
   
    return ApplyToClient("%s",sBerth);	
    
}

//在线查询票价
long dsTaskAirOnLine(char * inbuf)
{
    long nId,nRetVal;
    char outbuf[1024];
    char sAirLine[3],sRoute[7];
    double dSxf,dXsdjf,dSr;
    char cBerth[2];
    double dRebate,dSellbate,dPubbate,dBerbate;
    RECORD rec;
   
    FetchStringToArg(inbuf,"%s|%s|",sAirLine,sRoute);
     
    if(!strcmp(sRoute,"CTUXIY")) strcpy(sRoute,"CTUSIA");
    //printf("\n......COMPANY=%s,ROUTE=%s",sAirLine,sRoute);
    dSxf=dXsdjf=0.0;
    nId=RunSelect("select dsxf,dxsdjf from ticket.dl$air_company "
                  "where ccompany=%s into %lf%lf",sAirLine,&dSxf,&dXsdjf);
    dSxf+=dXsdjf;
    RunSelect("select dpubdate from ticket.dl$air_line where "
                   "cairline=%s into %lf ",sRoute,&dPubbate); 
    
    nId=OpenCursor("select distinct cberth from ticket.dl$company_rebate "
                           "where ccompany=%s",sAirLine);
                                  
    rec=CreateRecord();
    for(;;)
    {
        dSr=0.0;
    	if((nRetVal=FetchCursor(nId,"%s",cBerth))==0)
    	    break;
        if(RunSelect("select dberbate,drebate,dsellbate "
                   "from ticket.dl$company_rebate "
                   "where ccompany=%s AND "
                   "croute=%s AND "
                   "cberth=%s into %lf%lf%lf",
                   sAirLine,sRoute,cBerth,&dBerbate,&dRebate,&dSellbate)==0)        
           RunSelect("select dberbate,drebate,dsellbate "
                   "from ticket.dl$company_rebate "
                   "where ccompany=%s AND "
                   "croute='0' AND "
                   "cberth=%s  into %lf%lf%lf",
                   sAirLine,cBerth,&dBerbate,&dRebate,&dSellbate);
                   
        //printf("\n-->berbate:%lf|rebate:%lf|sellbate:%lf|",dBerbate,dRebate,dSellbate);
        if(dBerbate==0) continue;
        dBerbate=dPubbate*dBerbate/10;	
        dRebate=dPubbate*dRebate/10;
        dSellbate=dPubbate*dSellbate/10;
        dSr=(dBerbate-dRebate)+(dBerbate*dSxf/100);
        ROUND(&dBerbate,-1);
        ROUND(&dRebate,-1);
        ROUND(&dSellbate,-1);
        //ROUND(&dSr,-1);
        
        //printf("\n...SR=%lf",dSr);
    	ImportRecord(&rec,"%s%lf%lf%lf%lf%lf%lf",
    	            cBerth,dPubbate,dBerbate,dRebate,dSellbate,dSxf,dSr);			
    }
    CloseCursor(nId);
    return ApplyToClient("%r",rec);		
}//end fun

//检查有无历史数据
long dsTaskAirCheck(char * inbuf)
{
    RECORD rec;
    long nId,nRetVal,nflag=0;
    char buf[100];
    char route[7]=".";
    long nStime,nEtime;
    char sAirLine[10],sAirType[20],sBerth[60],sRoute[20];
    long nAirDate,nAirTime;

    strcpy(buf,inbuf);
    FetchStringToArg(buf,"%d|%d|%s|",&nAirDate,&nAirTime,route);
      
    rec=CreateRecord();    
    nId=OpenCursor("select cairline,croute,nstime,netime,cairtype,cberths "
                           "from ticket.dl$air_query "
                           "WHERE ndata=%ld AND ntime=%ld "
                           "AND croute=%s",
                            nAirDate,nAirTime,route);
 
    for(;;)
    {
       nRetVal=FetchCursor(nId,"%s%s%ld%ld%s%s",
                    sAirLine,sRoute,&nStime,&nEtime,sAirType,sBerth);
       if(nRetVal==0)
        break;
                        
       ImportRecord(&rec,"%s%s%ld%ld%s%s",
                        sAirLine,sRoute,nStime,nEtime,sAirType,sBerth);
	   nflag++;
    }
    if(nflag==0)
       ImportRecord(&rec,"%s%s%ld%ld%s%s","1","1",0,0,".",".");
    CloseCursor(nId);

    return ApplyToClient("%r",rec);
}

//订票
long dsTaskAirPurchase(char * inbuf)
{

    char buf[1024];
    char outbuf[1024];
    char sResult[1024];
    char sBuffer[1024];
    char sAction[1024];
    char sJym[16],cCs[4];
    char * offset;
    char sAirLsh[41],
         sAirLine[8],
         sRoute[8],
         sBerth[3],
         sGuestName[41],
         sAddress[41],
         sGuestPhone[21],
         sGuestId[19],
         sMemo[50],
         sJsmc[21],
         sCzydh[8],
         sFromCity[20],
         sDestCity[20],
         sAirType[10],
         sCompany[3];
    long nAirDate,
         st_time,
         nDay,
         nMonth,
         en_time,
         some_time,
         nFlag;
    double dPrice,
           dMinPrice,
           dFinalFee,
           dRebate,
           dDlSxf;
    char sTemp[12]="DZ:1";
           
    char * sMonth[12]={"JAN","FEB","MAR","APR","MAY","JUN",
                       "JUL","AUG","SEP","OCT","NOV","DEC"};
    long nRetVal;

    memset(outbuf,0,sizeof(outbuf));
    strcpy(buf,inbuf);
    
    offset=FetchStringToArg(buf,"%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%s|%s|%s|%f|%f|%f|%f|%s|%s|%s|%s|%d|",
                sAirLsh,sAirLine,sRoute,sFromCity,sDestCity,sAirType,sBerth,&nAirDate,&st_time,&en_time,sGuestName,sAddress,
                sGuestPhone,&dPrice,&dMinPrice,&dFinalFee,&dRebate,sJsmc,sCzydh,sGuestId,sMemo,&nFlag);

    fflush(stdout);
    if(!offset)
       return ApplyToClient("%s","001111|报文格式错误!");
    
    strncpy(sCompany,sAirLine,2);
    sCompany[2]='\0';
    if(!strcmp(sCompany,"3Q"))
       return ApplyToClient("%s","000111|云南航空，请与速递局联系！");
    if(!strcmp(sCompany,"XW") || !strcmp(sCompany,"2Z"))
       strcpy(sTemp,"DZ:1,HU");
    else strcpy(sTemp,"DZ:1");  //new struct

    dDlSxf=0.0;
    if(RunSelect("select dsxf from ticket.dl$air_company where "
			  "ccompany=%s into %lf",sCompany,&dDlSxf)<=0)
	   return ApplyToClient("%s","001110|你无权订此航空公司机票");
    fflush(stdout);

    strncpy(sJym,sAirLsh+9,15);
    sJym[15]='\0';
    
    //控制
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=9 into %s",
       			sJym,cCs);
    alltrim(cCs);
    if(!strcmp(cCs,"y") || !strcmp(cCs,"Y"))
       return ApplyToClient("%s","000111|该业务因故停止，请与速递局联系...|");
       
    nRetVal=dsInsertAirData(inbuf);
    fflush(stdout);
    if(nRetVal<0)
    {
    	GetSqlErrInfo(sAction);
    	RollbackWork();
    	sprintf(sResult,"%06ld|%s|",-nRetVal,sAction);
    	return ApplyToClient("%s",sResult);
    }
    
    RunSql("update dl$zwmx set ddlsxf=%lf,dytk=%lf where clsh=%s",
           dDlSxf,dFinalFee,sAirLsh);	

    nMonth=((nAirDate%10000)/100);
    nDay=nAirDate%100; 

    if(dRebate<10.0)
       sprintf(sAction,"YB%-ld",(long)(dRebate*10));
    else if(dRebate>10.0)
       sprintf(sAction,"FB");
    else sprintf(sAction,"YB");

    alltrim(sGuestName);
    
    switch(nFlag)
    {
		case 1:
			//建立PNR
			if(sBerth[0]=='Y' || sBerth[0]=='F')
			{
           		sprintf(sBuffer,"NM:1%s\n"
                    			"SS:%s/%s/%02ld%s/%s/RR1\n"
                    			"CT:%s\n"
                    			"FN:FCNY%-.2lf/SCNY%-.2lf/C%-.2lf\n"
                    			"FC:%-3.3s %-2.2s %-3.3s %-.2lf%s CNY%-.2lfEND\n"
                    			"FP:CASH,CNY\n"
                   		 		"%s\n",
                   	 			sGuestName,
                    			sAirLine,sBerth,nDay,sMonth[nMonth-1],sRoute,
                    			sGuestPhone,
                    			dFinalFee,dFinalFee,dDlSxf,
                    			sRoute,sCompany,sRoute+3,dFinalFee,sAction,dFinalFee,sTemp);
			}
			else
			{
				sprintf(sBuffer,"NM:1%s\n"
								"SS:%s/%s/%02ld%s/%s/RR1\n"
								"CT:%s\n"
								"FN:FCNY%-.2lf/SCNY%-.2lf/C%-.2lf\n"
								"FC:%-3.3s %-2.2s %-3.3s %-.2lf%s CNY%-.2lfEND\n"
								"FP:CASH,CNY\n"
								"EI:不得签转原出票地退票\n"
								"%s\n",
								sGuestName,
								sAirLine,sBerth,nDay,sMonth[nMonth-1],sRoute,
								sGuestPhone,
								dFinalFee,dFinalFee,dDlSxf,
								sRoute,sCompany,sRoute+3,dFinalFee,sAction,dFinalFee,sTemp);
			}break;
			
         case 2:
			//订座或手工出票PNR
			nMonth=((nAirDate%10000)/100);
			nDay=nAirDate%100;
			some_time=st_time-500;
         
			sprintf(sBuffer,"SD:1%s/1\n"
							"NM:1%s\n"
							"CT:%s\n"
							"TK:TL/%04ld/%02ld%s/CTU370\n"
							"@\n",
							sBerth,
							sGuestName,
							sGuestPhone,some_time,nDay,sMonth[nMonth-1]);

			break;
		
		default:  
			return ApplyToClient("%s","000111|所按控制键出错！|");
	}//end switch()
    
	nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
	if(nRetVal!=0)
	{
		GetAirErrInfo(nRetVal);
		sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
		RollbackWork();
		return ApplyToClient("%s",sResult);
	}
   
	switch(nFlag)
	{
		case 1:
			sprintf(sAction,"%.2lf",dFinalFee);
			if(offset=strstr(sResult,sAction))
			{
				while(offset[0]!=' ') offset++;
				while(offset[0]==' ') offset++;
				offset[5]='\0';
        
				RunSql("update dl$zwmx set cyhbz=%s where clsh=%s",offset,sAirLsh);
				RunSql("update dl$dbmx_%t set cvalue6=%s where clsh=%s",sJym,offset,sAirLsh);
			}
			else
			{
				RollbackWork();
				sprintf(sBuffer,"IG:\n");
				nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
				return ApplyToClient("111111|订票未知·请与速递局联系|");
			}//end if
			break;
			
		case 2:
			offset=sResult+52;
			if(strlen(offset)>5)
			{
				while(offset[0]!=' ') offset++;
				while(offset[0]==' ') offset++;
				while(offset[0]<=' ') offset++;
				offset[5]='\0';
				alltrim(offset);
			}
			if(strlen(offset)==5)
			{
				if(RunSql("update dl$dbmx_%t set cvalue6=%s,cvalue11=%s,"
							"cvalue12=%s,nvalue8=8,dvalue10=%.2lf where clsh=%s",
							sJym,offset,sCompany,sAction,
							dDlSxf,sAirLsh)>0)
					break;
			}
			else
			{
				RollbackWork();
				sprintf(sBuffer,"IG:\n");
				nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
				return ApplyToClient("111111|不能订购此票...|");
			}//end if
			break;
	}//end swi
	CommitWork();
	
	sprintf(outbuf,"000000|%s|",offset);
	//printf("\nTEST:AIRID[%ld]=%s",strlen(outbuf),outbuf);
	return ApplyToClient("%s",outbuf);
}

long dsTaskOutAirTicket(char * inbuf,long nFlag)
{
    char sResult[1024];
    char sBuffer[1024];
    char sJym[16];
    long nRetVal,nXh;
    char sYhbz[11],
         sLsh[41],
         sGuestName[41],
         sCzydh[8],
         sRoute[8],
         sAction[20],
         sCompany[3];
    double dFinalFee,dDlsxf,dPrice,dRebate=0;
    
    
    FetchStringToArg(inbuf,"%s|%s|",sJym,sYhbz);
    
    if(nRetVal=RunSelect("select clsh from dl$dbmx_%t where cvalue6=%s "
			"and ncsbz!=8 into %s",sJym,sYhbz,sLsh)<=0)
	   	return ApplyToClient("%s","001111|没有相应的订票记录...|");

    switch(nFlag)
    {
       case 1:
       sprintf(sBuffer,"RT %s\nXEPNR@\n",sYhbz);    
       break;
       
       case 2:
       if(RunSelect("select cvalue2,cvalue7,cvalue11,cczydh,"
                    "cvalue12,dvalue3,dvalue4,dvalue10 from dl$dbmx_%t "
                    "where clsh=%s AND cvalue6=%s "
				    "into %s%s%s%s%s%lf%lf%lf",
                        sJym,sLsh,sYhbz,
                        sGuestName,sRoute,sCompany,sCzydh,
                        sAction,&dFinalFee,&dRebate,&dDlsxf)<=0)
           return ApplyToClient("%s","111111|取得票据参数出错...|");
          
       //printf("\nTEST:%s|%s|%s|%s|%lf|%lf|%lf|",sGuestName,sRoute,sCompany,sAction,dFinalFee,dDlsxf);
       alltrim(sRoute),alltrim(sCompany),alltrim(sAction);
       
       if(dRebate>=10)
       {
            sprintf(sBuffer,"RT:%s\n"
                       "XE:5\n"
                       "FN:FCNY%-.2lf/SCNY%-.2lf/C%-.2lf\n"
                       "FC:%-3.3s %-2.2s %-3.3s %-.2lf%s CNY%-.2lfEND\n"
                       "FP:CASH,CNY\n"
                       "DZ:1\n",
                        sYhbz,
                        dFinalFee,dFinalFee,dDlsxf,             
                        sRoute,sCompany,sRoute+3,dFinalFee,sAction,dFinalFee);
       }
       else
       {
            sprintf(sBuffer,"RT:%s\n"
                       "XE:5\n"
                       "FN:FCNY%-.2lf/SCNY%-.2lf/C%-.2lf\n"
                       "FC:%-3.3s %-2.2s %-3.3s %-.2lf%s CNY%-.2lfEND\n"
                       "FP:CASH,CNY\n"
			"EI:不得签转原出票地退票\n"
                       "DZ:1\n",
                        sYhbz,
                        dFinalFee,dFinalFee,dDlsxf,             
                        sRoute,sCompany,sRoute+3,dFinalFee,sAction,dFinalFee);
       }
       break;
       default: return ApplyToClient("%s","111111|传送参数不正确...|");
    }//end swi
    
    fflush(stdout);
    nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
    if(nRetVal!=0)
    {
       GetAirErrInfo(nRetVal);
       sprintf(sBuffer,"IG:\n");
       nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
       if(nRetVal)
       {
         sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
         RollbackWork();
       }
       return ApplyToClient("%s",sResult);
    }
    
    switch(nFlag)
    {
        case 1:
        RunSql("update dl$dbmx_%t set cvalue5=%s where clsh=%s",sJym,'*',sLsh);
		nRetVal=RunSql("update dl$zwmx set cyhbz=%s,nvalue8=1,ncsbz=8,dsjk=0 "
				"where clsh=%s ",sYhbz,sLsh);
        break;
        
        case 2:
        if(strstr(sResult,sYhbz))
        {
			nRetVal=RunSql("update dl$zwmx set cyhbz=%s,ncsbz=1,nvalue8=0 "
					"where clsh=%s ",sYhbz,sLsh);
		}
		else
		{
	   		sprintf(sResult,"111111|出票未知,请与速递局联系(%s)|",sYhbz);
	   		return ApplyToClient("%s",sResult);  
		}
		break;
        
        default: return ApplyToClient("%s","111111|传送参数不正确...|");
    }     
    if(nRetVal>0)
         CommitWork();
    else RollbackWork();

    sprintf(sResult,"000000|成功处理.%s|",sLsh);
    return ApplyToClient("%s",sResult);  
   
}

//取打印数据
long dsTaskAirConfirm(char * inbuf)
{
    long nRetVal;
    char sJym[16];
    char sResult[1024];
    char sJsdh[10],sCzy[8],sName[16],sId[19],sPhone[18],sAdd[25];
    char sFcity[16],sDcity[16],sBerth[2],sType[4],sAirId[6],sMemo[20];
    long nLkrq,nTime,nRet;
    double dBerbate,dFinFree;

    strncpy(sJym,inbuf+9,15);
    sJym[15]='\0';
	nRetVal=RunSelect("select cvalue3,cczydh,cvalue2,cvalue4,cvalue9,\
						cvalue1,cvalue13,cvalue8,cvalue15,nvalue1,nvalue2,\
						 cvalue6,cvalue14,cvalue5,dvalue2,dvalue3 \
						 from dl$dbmx_%t where clsh=%s \
						 into %s%s%s%s%s,%s%s%s%s%ld%ld,%s%s%s%lf%lf ",
						 sJym,inbuf,
						 sJsdh,sCzy,sName,sId,sPhone,
						 sAdd,sFcity,sBerth,sType,&nLkrq,&nTime,
						 sAirId,sDcity,sMemo,&dBerbate,&dFinFree);
	if(nRetVal>0)
	   sprintf(sResult,"000000|%s|%s|%s|%s|%s|%s|%s|%s|%s|%ld|%ld|%s|%s|%s|%lf|%lf|",
	   					 sJsdh,sCzy,sName,sId,sPhone,
						 sAdd,sFcity,sBerth,sType,nLkrq,nTime,
						 sAirId,sDcity,sMemo,dBerbate,dFinFree);
    else sprintf(sResult,"111111|没有此条记录信息!");
    
    //printf("\nTEST:nId=%ld,JYM=%s\n,RET<%s>",nRetVal,sJym,sResult);						 
	return ApplyToClient("%s",sResult);
}

//插入业务明细
static long dsInsertAirData(char * inbuf)
{

    char sBuffer[1024];
    char sJym[16];
    char * offset;
    char sAirLsh[41],
         sAirLine[8],
         sRoute[8],
         sBerth[3],
         sGuestName[41],
         sAddress[41],
         sGuestPhone[21],
         sJsmc[21],
         sGuestId[19],
         sMemo[50],
         sJsdh[10],
         sFromCity[20],
         sDestCity[20],
         sAirType[10],
         sCzydh[8];
    char sTicketIdent[21];
    long nAirDate,
         st_time,
         en_time,
         nFlag=1;
    double dPrice,
           dMinPrice,
           dFinalFee,
           dRebate;
    long nRetVal,nXh=0;
       
    GetSysLkrq();
    offset=FetchStringToArg(inbuf,"%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%s|%s|%s|%f|%f|%f|%f|%s|%s|%s|%s|%ld|",
                sAirLsh,sAirLine,sRoute,sFromCity,sDestCity,sAirType,sBerth,&nAirDate,&st_time,&en_time,sGuestName,sAddress,
                sGuestPhone,&dPrice,&dMinPrice,&dFinalFee,&dRebate,sJsmc,sCzydh,
                sGuestId,sMemo,&nFlag);
    
    strcpy(sTicketIdent,".");
    if(strlen(offset)>=5) FetchStringToArg(offset,"%s|",sTicketIdent);

    FetchStringToArg(sAirLsh,"%9s%15s",sJsdh,sJym);

    nRetVal = RunSql("INSERT INTO DL$DBMX_%t(CLSH,NLKRQ,NSFM,CCZYDH,"
       "CVALUE1,CVALUE2,CVALUE3,CVALUE4,CVALUE5,"
       "CVALUE6,CVALUE7,CVALUE8,CVALUE9,CVALUE10,"
       "CVALUE11,CVALUE12,CVALUE13,CVALUE14,CVALUE15,"
       "NVALUE1,NVALUE2,NVALUE3,NVALUE4,NVALUE5,"
       "NVALUE6,NVALUE7,NVALUE8,NVALUE9,NVALUE10,"
       "DVALUE1,DVALUE2,DVALUE3,DVALUE4,DVALUE5,"
       "DVALUE6,DVALUE7,DVALUE8,DVALUE9,DVALUE10) "
       "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
		"%s,%s,%s,%s,%s,"
       "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
       "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
       sJym,sAirLsh,nSysLkrq,nSysSfm,sCzydh,
       sAddress,sGuestName,sJsdh,sGuestId,sMemo,
       sTicketIdent,sRoute,sBerth,sGuestPhone,sJsmc,
	   ".",".",sFromCity,sDestCity,sAirType,
       nAirDate,st_time,en_time,0,0,0,0,0,0,0,
       dPrice,dMinPrice,dFinalFee,dRebate,0.0,0.0,0.0,0.0,0.0,0.0);
    
    if(nRetVal<0) return nRetVal;
    
    RunSelect("select dlwtlsh.nextval from dual into %ld",&nXh);
    sprintf(sBuffer,"%s|%s|%s|1|%.2lf|%s|%ld|0|",
           		sAirLsh,sTicketIdent,sGuestName,dFinalFee,sCzydh,nSysSfm);
	nRetVal = InsertZwmxTable(sBuffer,TASK_PAY_CONFIRM,nXh);

		/*if(nRetVal>0)
 		RunSql("update dl$zwmx set dyjzk=%lf where clsh=%s",dFinalFee,sAirLsh);*/	
    
    return nRetVal; 	
}

//退票处理:
long dsAirRecall(char * inbuf,long nxh,char * outbuf)
{
    char sResult[1024];
    char sGateWay[31]=".";
    
    RunSelect("select sgateway from dl$route where sdestination='dlsj' into %s",sGateWay);
    
    if(strcmp(sGateWay,"localhost"))
    {
    	if(AutoRunTask("dlsj",TASK_AIR_RETURN_TICKET,"%s","%s",inbuf,sResult)) return 0;
    	strcpy(outbuf,sResult);
  		return 0;
    }
    else 
    	return  dsTaskAirReturnTicket(inbuf);

}

long dsTaskAirReturnTicket(char * inbuf)
{
    char sBuffer[1024];
    char sResult[1024];
    char sTicketCode[21]=".";
    char sLsh[41],sRecallLsh[41];
    long nRetVal;
    
    
    FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
    
    if(RunSelect("select cyhbz from dl$zwmx where clsh=%s into %s",sRecallLsh,sTicketCode)<=0)
      return ApplyToClient("%s","001111|没有相应的订票记录...|");
    

    sprintf(sBuffer,"RT %s\nXEPNR@\n",sTicketCode);    
    /////////////////进入发送临介区////////////////
    nRetVal = dsAirSendAndRecv(-1,sBuffer,sResult);
    if(nRetVal!=0)
    {
       GetAirErrInfo(nRetVal);
       sprintf(sResult,"%06ld|%s|",-nRetVal,sTcpipErrInfo);
       RollbackWork();
       return ApplyToClient("%s",sResult);
    }
    /////////////////结束发送临介区//////////////////
    if(!strstr(sResult,"CANCELLED"))
    {
       sprintf(sBuffer,"002312|%s",sResult);
       RollbackWork();
       return ApplyToClient("%s",sBuffer);
    }

    dsModifyZwmxOnRecall(inbuf,sResult);
    if(strncmp(sResult,"000000",6)) RollbackWork();
    else CommitWork();
    return ApplyToClient("%s",sResult);
}

long dsTaskAirSignIn(char * sJym)
{
    long nRetVal;
    char sResult[1024];
    char sAction[1024];
    char sUserName[21]=".",sPassWord[21]=".";
    
    RunSelect("select ccs from ticket.dl$jymcsfb where nbh=1 and cjym=%s into %s",sJym,sUserName);
    RunSelect("select ccs from ticket.dl$jymcsfb where nbh=2 and cjym=%s into %s",sJym,sPassWord);
    sprintf(sAction,"SI %s/%s\n",sUserName,sPassWord);
    nRetVal = dsAirSendAndRecv(-1,sAction,sResult);
    if(nRetVal)
    {
        GetAirErrInfo(nRetVal);
        return ApplyToClient("%s",sTcpipErrInfo);
    }
    return ApplyToClient("%s",sResult);    
}

long dsTaskAirSignOut()
{
    long nRetVal;
    nRetVal = dsAirSendAndRecv(-1,"SO\n",NULL);
    if(nRetVal)
    {
        GetAirErrInfo(nRetVal);
        return ApplyToClient("%s",sTcpipErrInfo);
    }
    return ApplyToClient("%s","签退完毕...");    
}
//以上暂日未用

//通信处理
static int dsAirSendAndRecv(int nsock,char * inbuf,char * outbuf)
{
    
    long nClose=0;
    long nRetVal;
    
    printf("\nSEND:%s",inbuf);
    if(nsock<0)
    {
       nsock = connectTCP("dl_tx_8","air");
       if(nsock<0) return -14;
       nClose=1;
    }

    if(SendStringToSocket(nsock,inbuf))
    {
       close(nsock);
       return -14;
    }	

    if(outbuf==NULL)
    {
    	if(nClose==1) close(nsock);
    	return 0;
    }
    if(GetLongFromSocket(nsock,&nRetVal))
    {
    	close(nsock);
    	return -14;
    }

    if(nRetVal!=0)
    {
    	close(nsock);
    	return nRetVal;
    }
    
    if(GetStringFromSocket(nsock,outbuf))
    {
    	close(nsock);
    	return -14;
    }
    
    if(nClose==1) close(nsock);
    printf("\nRECV:%s",outbuf);
    return 0;
}

static void GetAirErrInfo(long nRetCode)
{

   	switch(nRetCode)
   	{
      case -1:
           strcpy(sTcpipErrInfo,"查询事务出错[ERR_NO_SESSION]");
           break;
      case -4:
           strcpy(sTcpipErrInfo,"查询返回出错[ERR_RET]");
           break;
      case -5:
           strcpy(sTcpipErrInfo,"SOCKET错误[ERR_SOCKET]");
           break;
      case -6:
           strcpy(sTcpipErrInfo,"事务已关闭[ERR_CLOSED]");
           break;
      case -7:
           strcpy(sTcpipErrInfo,"错误务的事务ID[ERR_SESSION_ID]");
           break;
      case -8:
           strcpy(sTcpipErrInfo,"缓冲区溢出[ERR_LEN_OVER]");
           break;
      case -12:
           strcpy(sTcpipErrInfo,"等待民航方返回数据超时");
           break;
      default:
           strcpy(sTcpipErrInfo,"与通信机通信失败");
           break;
    }            
}

//过滤无座舱位信息
static long fillterberth(char source[],char sout[])
{
	int i,j=-2,k=0;
  
  	for(i=0;i<strlen(source);i++)
  	{
    	sout[k++]=source[i];
     	if((i-j)==3)
     	{
       		j=i;
       		if(source[i]>'A' && source[i]<='Z')
         	k-=3;
     	}
  	}
	if(source[i-2]>'A') sout[k-1]='\0';
	else sout[k]='\0';
}

//查询机票
long dsTaskQueryAirTicket(char * inbuf)
{
    char sResult[1024];
    char sGateWay[31]=".";
    RECORD rec;
    long nId,nRetVal;
	long nSt_time,nZt;
	char sJym[16]=".";
	char sAirId[6]=".",sName[22]=".",sFromCity[21]=".",sDestCity[21]=".",sBerth[4]=".",sPhone[20]=".";
	char sMemo[51]=".",sJsdh[10]=".",sJm[21]=".",sCzydh[7]=".";
	char sAirLineC[20]=".";
	char sLsh[41]=".";
	char sJsdh1[7]=".",sJsdh2[31]=".";
	long date=0,nJymYwjb=0;

   	FetchStringToArg(inbuf,"%s|%s|%d|%d",sJym,sJsdh,&date,&nJymYwjb);

   	rec=CreateRecord();   
   	if(!strncmp(sJsdh,sJym+9,6))
   		sprintf(sJsdh2," ");
   	else
   	{
   		if(nJymYwjb<=2)
   		{
			if(strncmp(sJsdh+4,"01",2)==0)
			{
				strncpy(sJsdh1,sJsdh,4);
				sJsdh1[4]='\0';
				sprintf(sJsdh2,"and substr(mx.cvalue3,1,4)=%s",sJsdh1);
			}
			else
			{
				strncpy(sJsdh1,sJsdh,6);
				sJsdh1[6]='\0';
				sprintf(sJsdh2,"and substr(mx.cvalue3,1,6)=%s",sJsdh1);
			}
		}
		else
		{
			strncpy(sJsdh1,sJsdh,6);
    		sJsdh1[6]='\0';
    		sprintf(sJsdh1,"and substr(mx.cvalue3,1,6)=%s",sJsdh1);
    	}
    }

    nId=OpenCursor("select mx.clsh,mx.cvalue2,mx.cvalue6,mx.cvalue5,mx.cvalue13,mx.cvalue14,"
    				"mx.cvalue8,mx.cvalue9,mx.cvalue10,mx.cczydh,mx.nvalue2,mx.nvalue10 "
    		  		"from dl$dbmx_%t mx,dl$zwmx zw "
    		  		"where mx.cvalue5!='*' and "
    		  		"mx.cvalue6!='*' and mx.nlkrq=%ld %t "
    		  		"and mx.clsh=zw.clsh order by mx.nsfm",sJym,date,sJsdh2);
    if(nId<0)
		return ApplyToClient("%ld%r",-1,rec);
		
	for(;;)
   	{
		if(FetchCursor(nId,"%s%s%s%s%s%s%s"
							"%s%s%s%ld%ld",
				sLsh,sName,sAirId,sMemo,sFromCity,sDestCity,
				sBerth,sPhone,sJm,sCzydh,&nSt_time,&nZt)<=0) break;
 
    	if(strncmp(sJsdh,sJym+9,6)==0&&nZt==1)
    		nZt=3;

    	ImportRecord(&rec,"%s%s%s%s%s%s%s"
    					"%s%s%s%ld%ld",
    			sLsh,sName,sAirId,sMemo,sFromCity,sDestCity,
    			sBerth,sPhone,sJm,sCzydh,nSt_time,nZt);
   	}
   	
   	CloseCursor(nId);
   	return ApplyToClient("%ld%r",0,rec);
}


//数据源局审核机票
long dsTaskLoadCheckTicketData(char *inbuf)
{
    RECORD rec;
    long nId,nRetVal;
    long nSt_time;
    char sJym[16]=".",sCzydh[7]=".";
    char sAirId[6]=".",sName[22]=".",sFromCity[21]=".",sDestCity[21]=".";
    char sBerth[4]=".",sPhone[20]=".";
    char sAirLineC[20]=".";
    char sLsh[41]=".";
   	long nLkrq=0;

    FetchStringToArg(inbuf,"%s|",sJym);

    rec=CreateRecord();    

	nId=OpenCursor("select mx.clsh,mx.cvalue2,zw.cyhbz,mx.cvalue13,mx.cvalue14,"
					"mx.cvalue8,mx.nvalue2,mx.nlkrq "
					"from dl$dbmx_%t mx,dl$zwmx zw "
					"where mx.clsh=zw.clsh and mx.nvalue10=0 and zw.ncsbz=1 "
					"order by mx.nsfm",sJym);
    if(nId<0)
    	return ApplyToClient("%ld%r",-1,rec);;

    for(;;)
    {
		nRetVal=FetchCursor(nId,"%s%s%s%s%s%s%ld%ld",
				sLsh,sName,sAirId,sFromCity,sDestCity,sBerth,&nSt_time,&nLkrq);
    	if(nRetVal==0) break;
    	
    	
    	ImportRecord(&rec,"%s%s%s%s%s%s%ld%ld",
    			sLsh,sName,sAirId,sFromCity,sDestCity,sBerth,nSt_time,nLkrq);
    }
    CloseCursor(nId);
    return ApplyToClient("%ld%r",1,rec);
}

//数据源局审核机票入库
long dsTaskSaveCheckResult(char *inbuf)
{
	long nRetVal;
	char sLsh[41]=".",sAirId[6]=".";
	long nCount,i;    
	char *offset;
	char sJym[16]=".";
        
	offset=FetchStringToArg(inbuf,"%s|%d|",sJym,&nCount);
	if(nCount<1)  return ApplyToClient("%s","000111|没有相应记录..|");

	for(i=1;i<=nCount;i++)
	{
    	offset=FetchStringToArg(offset,"%s|%s|",sAirId,sLsh);

		nRetVal=RunSql("update dl$dbmx_%t set cvalue6=%s,nvalue10=1 "
						"where clsh=%s ",sJym,sAirId,sLsh);
		nRetVal=RunSql("update dl$zwmx set cyhbz=%s,ncsbz=2 "
						"where clsh=%s ",sAirId,sLsh);
		if(nRetVal<0) break;
	}
   
	if(nRetVal<0)
	{
		RollbackWork();
		return ApplyToClient("%s","入库出错");
	}
	else
	{
		CommitWork();
		return ApplyToClient("%s","入库成功");
	}
}

//非数据源局获取业务明细数据
long dsTaskGetDesAirData(char *inbuf)
{
    RECORD rec;
    long nId,nRetVal;
    char sBuffer[1024];
    char sJym[16]=".";
    long date=0;
    char sAirLsh[41],
         sAirLine[8],
         sRoute[8],
         sBerth[3],
         sGuestName[41],
         sAddress[41],
         sGuestPhone[21],
         sJsmc[21],
         sGuestId[21],
         sMemo[51],
         sJsdh[10],
         sFromCity[21],
         sDestCity[21],
         sAirType[21],
         sCzydh[8];
    char sTicketIdent[21];
    long nAirDate,
         st_time,
         en_time,
         nSfm,
         nLkrq,
         nFlag=1;
    double dPrice,
           dMinPrice,
           dFinalFee,
           dRebate;
    
    long nJymYwjb;       
	char sJymHost[41],sJsdh1[7];

    FetchStringToArg(inbuf,"%s|%s|%d|",sJym,sJsdh,&nJymYwjb);

	//省级、市级业务
	if(nJymYwjb<=2)
	{
		strncpy(sJsdh1,sJsdh,4);
		sJsdh1[4]='\0';
	
		//恢复已取数据但未返回对帐成功的标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=1 "
						"where nvalue10=2 and cvalue6!='*' "
						"and substr(cvalue3,1,4)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			RollbackWork();
			return ApplyToClient("%ld%r",-1,rec);
		}
		
		//提取未对帐数据
		nId=OpenCursor("select CLSH,NLKRQ,NSFM,CCZYDH,"
					"CVALUE1,CVALUE2,CVALUE3,CVALUE4,CVALUE5,"
					"CVALUE6,CVALUE7,CVALUE8,CVALUE9,CVALUE10,"
					"CVALUE13,CVALUE14,CVALUE15,"
					"NVALUE1,NVALUE2,NVALUE3,"
					"DVALUE1,DVALUE2,DVALUE3,DVALUE4 "
					"from dl$dbmx_%t "
					"where nvalue10=1 and cvalue6!='*' "
					"and substr(cvalue3,1,4)=%s ",sJym,sJsdh1);
		if(nId<0)
		{
			RollbackWork();
			return ApplyToClient("%ld%r",-1,rec);
		}
		
		
		//标记已取数据标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=2 "
						"where nvalue10=1 and cvalue6!='*' "
						"and substr(cvalue3,1,4)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			CloseCursor(nId);
			RollbackWork();
			return ApplyToClient("%ld%r",-1,rec);
		}
	}
	//县级业务
	else
	{
		strncpy(sJsdh1,sJsdh,6);
		sJsdh1[6]='\0';
		//alltrim(sJsdh1);
		
		//恢复已取数据但未返回对帐成功的标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=1 "
						"where nvalue10=2 and cvalue6!='*' "
						"and substr(cvalue3,1,4)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			RollbackWork();
			return ApplyToClient("%ld%r",-1,rec);
		}
		
		//提取未对帐数据
		nId=OpenCursor("select CLSH,NLKRQ,NSFM,CCZYDH,"
					"CVALUE1,CVALUE2,CVALUE3,CVALUE4,CVALUE5,"
					"CVALUE6,CVALUE7,CVALUE8,CVALUE9,CVALUE10,"
					"CVALUE13,CVALUE14,CVALUE15,"
					"NVALUE1,NVALUE2,NVALUE3,"
					"DVALUE1,DVALUE2,DVALUE3,DVALUE4 "
					"from dl$dbmx_%t "
					"where nvalue10=1 and cvalue6!='*' "
					"and substr(cvalue3,1,6)=%s ",sJym,sJsdh1);
		if(nId<0)
		{
			RollbackWork();		
			return ApplyToClient("%ld%r",-1,rec);
		}
		
		//标记已取数据标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=2 "
						"where nvalue10=1 and cvalue6!='*' "
						"and substr(cvalue3,1,6)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			CloseCursor(nId);
			RollbackWork();
			return ApplyToClient("%ld%r",-1,rec);
		}
	}
	CommitWork();
	
	rec=CreateRecord();        
	while(1)
	{
		nRetVal=FetchCursor(nId,
					"%s%ld%ld%s"
					"%s%s%s%s%s"
					"%s%s%s%s%s"
					"%s%s%s"
					"%ld%ld%ld"
					"%lf%lf%lf%lf",
					sAirLsh,&nLkrq,&nSfm,sCzydh,
					sAddress,sGuestName,sJsdh,sGuestId,sMemo,
					sTicketIdent,sRoute,sBerth,sGuestPhone,sJsmc,
					sFromCity,sDestCity,sAirType,
					&nAirDate,&st_time,&en_time,
					&dPrice,&dMinPrice,&dFinalFee,&dRebate);
		if(nRetVal==0) break;
	 
		ImportRecord(&rec,"%s%ld%ld%s"
					"%s%s%s%s%s"
					"%s%s%s%s%s"
					"%s%s%s"
					"%ld%ld%ld"
					"%lf%lf%lf%lf",
					sAirLsh,nLkrq,nSfm,sCzydh,
					sAddress,sGuestName,sJsdh,sGuestId,sMemo,
					sTicketIdent,sRoute,sBerth,sGuestPhone,sJsmc,
					sFromCity,sDestCity,sAirType,
					nAirDate,st_time,en_time,
					dPrice,dMinPrice,dFinalFee,dRebate);
	 
	}//end_dbmx
	CloseCursor(nId);
	return ApplyToClient("%ld%r",1,rec);
}

//数据源局处理非数据源局对帐返回
long dsTaskCheckReturnDesAirTicket(char *inbuf)
{
    long nId,nRetVal;
    char sJym[16]=".",sJsdh[10]=".";
    long nJymYwjb=0;       
	char sJsdh1[10]=".";

    FetchStringToArg(inbuf,"%s|%s|%d|",sJym,sJsdh,&nJymYwjb);

	//省级、市级业务
	if(nJymYwjb<=2)
	{
		strncpy(sJsdh1,sJsdh,4);
		sJsdh1[4]='\0';
		//alltrim(sJsdh1);

		//对帐成功的返回标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=3 "
						"where nvalue10=2 and cvalue6!='*' "
						"and substr(cvalue3,1,4)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			RollbackWork();
			return ApplyToClient("%ld",-1);
		}
	}
	//县级业务
	else
	{
		strncpy(sJsdh1,sJsdh,6);
		sJsdh1[6]='\0';
		//alltrim(sJsdh1);

		//对帐成功的返回标志
		nRetVal=RunSql("update dl$dbmx_%t set nvalue10=3 "
						"where nvalue10=2 and cvalue6!='*' "
						"and substr(cvalue3,1,6)=%s ",sJym,sJsdh1);
		if(nRetVal<0)
		{
			RollbackWork();
			return ApplyToClient("%ld",-1);
		}
	}
	CommitWork();
	return ApplyToClient("%ld",0);
}

//非数据源局机票对帐
long dsTaskCheckDesAirTicket(char *inbuf)
{
    long nRetVal;
    char sAirLsh[41],
         sAirLine[8],
         sRoute[8],
         sBerth[3],
         sGuestName[41],
         sAddress[41],
         sGuestPhone[21],
         sJsmc[21],
         sGuestId[21],
         sMemo[51],
         sJsdh[10],
         sFromCity[21],
         sDestCity[21],
         sAirType[21],
         sCzydh[8];
    char sTicketIdent[21];
    char sJym[16]=".",sBuffer[1024]=".";
	long nAirDate,
		nSfm,
		nLkrq,
		st_time,
		en_time,
		date,
		nXh,nFlag=1;
    double dPrice,
           dMinPrice,
           dFinalFee,
           dRebate;
	long nRetTask,nLkrq1;
	long nJymYwjb;  
	char sJymHost[41];
	RECORD rec1,rec2;
    
	FetchStringToArg(inbuf,"%s|%s|%s|%d|",sJym,sJymHost,sJsdh,&nJymYwjb);
	inbuf[0]='\0';
	sprintf(inbuf,"%s|%s|%d|",sJym,sJsdh,nJymYwjb);
	
	//提取远方对帐数据
	rec2=CreateRecord();
	nRetVal=AutoRunTask(sJymHost,TASK_GET_DES_AIRTICKET,"%s","%ld%r",inbuf,&nRetTask,&rec1);
	if(nRetVal) return;
	if(nRetTask==-1)
		return ApplyToClient("%ld%r",-1,rec2);

    for(;;)
    {
		if(ExportRecord(&rec1,"%s%ld%ld%s"
							"%s%s%s%s%s"
							"%s%s%s%s%s"
							"%s%s%s"
							"%ld%ld%ld"
							"%lf%lf%lf%lf",
							sAirLsh,&nLkrq,&nSfm,sCzydh,
							sAddress,sGuestName,sJsdh,sGuestId,sMemo,
							sTicketIdent,sRoute,sBerth,sGuestPhone,sJsmc,
							sFromCity,sDestCity,sAirType,
							&nAirDate,&st_time,&en_time,
							&dPrice,&dMinPrice,&dFinalFee,&dRebate)<=0) break;
		if(sAirLsh[0]=='.') break;
           
		//代办明细的入库
		nRetVal=RunSelect("select nlkrq from dl$dbmx_%t where clsh=%s into %ld",
							sJym,sAirLsh,&nLkrq1);
		if(nRetVal>0) continue;

		nRetVal=RunSql("insert into dl$dbmx_%t(CLSH,NLKRQ,NSFM,CCZYDH,"
						"CVALUE1,CVALUE2,CVALUE3,CVALUE4,CVALUE5,"
						"CVALUE6,CVALUE7,CVALUE8,CVALUE9,CVALUE10,"
       					"CVALUE11,CVALUE12,CVALUE13,CVALUE14,CVALUE15,"
						"NVALUE1,NVALUE2,NVALUE3,NVALUE4,NVALUE5,"
						"NVALUE6,NVALUE7,NVALUE8,NVALUE9,NVALUE10,"
						"DVALUE1,DVALUE2,DVALUE3,DVALUE4,DVALUE5,"
						"DVALUE6,DVALUE7,DVALUE8,DVALUE9,DVALUE10) "
						"values(%s,%ld,%ld,%s,"
						"%s,%s,%s,%s,%s,"
						"%s,%s,%s,%s,%s,"
						"%s,%s,%s,%s,%s,"
						"%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
						"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
						sJym,sAirLsh,nLkrq,nSfm,sCzydh,
						sAddress,sGuestName,sJsdh,sGuestId,sMemo,
						sTicketIdent,sRoute,sBerth,sGuestPhone,sJsmc,
						".",".",sFromCity,sDestCity,sAirType,
						nAirDate,st_time,en_time,0,0,0,0,0,0,0,
						dPrice,dMinPrice,dFinalFee,dRebate,0.0,0.0,0.0,0.0,0.0,0.0);
		if(nRetVal<0)
		{
			DropRecord(&rec1);
			RollbackWork();		
			return ApplyToClient("%ld%r",-3,rec2);
		} 
		
		//帐务明细入库
		memset(sBuffer,'\0',strlen(sBuffer));
		//增加异地交易码和局所代号！
		sprintf(sBuffer,"%s|%s|%s|%s|%s|1|%.2lf|%s|%ld|%ld|0|",
						sJym,sJsdh,sAirLsh,sTicketIdent,sGuestName,dFinalFee,sCzydh,nLkrq,nSfm);

		RunSelect("select dlwtlsh.nextval from dual into %ld",&nXh);
		//nRetVal = InsertDesZwmxTable(sBuffer,TASK_PAY_CONFIRM,nXh);
		if(nRetVal<0)
		{
			DropRecord(&rec1);
			RollbackWork();			
			return ApplyToClient("%ld%r",-2,rec2);
		}

		//下传记录
		ImportRecord(&rec2,"%s%s%s%s%s%s%s%s%ld%ld",
						sAirLsh,sGuestName,sTicketIdent,sFromCity,sDestCity,
						sBerth,sCzydh,sJsmc,st_time,nLkrq);
	}//end for()
	DropRecord(&rec1);
	CommitWork();
	
	//对帐结果成功送返
	nRetVal=AutoRunTask(sJymHost,TASK_CHECK_RETURN_DES_AIRTICKET,"%s","%ld",inbuf,&nRetTask);
	if(nRetVal) return;
	if(nRetTask==-1)
		return ApplyToClient("%ld%r",-4,rec2);

	return ApplyToClient("%ld%r",0,rec2); 	
}

//四舍五入，只能从(百位--百分位):参数(-2~~2)
void ROUND(double * pVar,int point)
{
  double dVar1,
         dVar2;
  
  if(point==-2)
     *pVar/=100;
  else if(point==-1)
     *pVar/=10;
  else if(point==1)
     *pVar*=10;
  else if(point==2)
     *pVar*=100;
  else return;
  
  if (*pVar>=0.0)
  {
    *pVar+=0.5;
  }
  else
  {
    *pVar-=0.5;
  } // end if

  dVar2=modf(*pVar,&dVar1);

  if(point==-2)
     *pVar=dVar1*100;
  else if(point==-1)
     *pVar=dVar1*10;
  else if(point==1)
    *pVar=dVar1/10;
  else if(point==2)
  	*pVar=dVar1/100;
    
} // ROUND

//省局入接收
long dsTaskSaveAirPay(char *inbuf)
{
	long nRetVal;
	char sLsh[41]=".";
	long nCount,i,nJsbz=0;    
	char buf[10]=".";
	char *offset;
	        
	if(strncmp(inbuf,"000000",6))  return ApplyToClient("%s","111111|接收包不正确|");
	offset=FetchStringToArg(inbuf,"%s|%d|",buf,&nCount);
    fflush(stdout);
    if(nCount<1)  return ApplyToClient("%s","000111|没有相应记录..|");

    for(i=1;i<=nCount;i++)
	{
    	offset=FetchStringToArg(offset,"%s|%d|",sLsh,&nJsbz);
    	//printf("TEST:|%s|%d|",sLsh,nJsbz);
		if(!offset)  break;
		nRetVal=RunSql("update dl$zwmx set njsbz=%ld where clsh=%s ",nJsbz,sLsh);
		if(nRetVal<0) break;
	}
   
	if(nRetVal<0)
	{
		RollbackWork();
		return ApplyToClient("%s","111011|上传失败，打电话到速递局核对！");
	}
	else
	{
		CommitWork();
		return ApplyToClient("%s","000000|上传成功，打电话到速递局核对！");
	}
}
