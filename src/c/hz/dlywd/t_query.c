#include"dlywd.h"

extern long nSysDebug;
extern long nSysLkrq;
extern long nSysSfm;
/*============================================================
�������ɷѲ�ѯ����˳���TASK_PAY_QUERY
���ܣ������û�Ҫ���ָ�����������������ݳ���ģʽ��ncxms������ѯ
      �����ض��򣬲�����ѯ�����֯��ͳһ�ĸ�ʽ���ظ��ͻ���
������inbuf:��������ַ���
      ��ˮ��|��������λͼ|������������|����Ա����|
    char(40)  char(5)      char(<=100)
      ��ˮ�ţ�[9λ�ֺ�+15λ������+8λ����+8λ���]
���أ�>0�ɹ�
      [����RECORD]
      �ɹ���000000|�û�����|��ͬ��|Ԥ����|��ϸ��¼��|{�û���־|���|�·�|25���շ���ϸ...
           |����Ӧ��|4���������...|4���ַ���������|}....
      ʧ�ܣ�XXXXXX|������Ϣ���ֽ���|
*============================================================*/
long dsTaskPayQuery(char * inbuf)
{
	
     RECORD Client;    //���ؿͻ������ݼ�¼
     
     long nCxms,nRetVal;      //����ģʽ
     char sJym[16];//������
     char sLsh[41];
     char sMap[6];
     char sJsdh[10];
     	
     GetSysLkrq();
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sMap);
     

     if(!strchr(sMap,'1'))
     {
     	ApplyToClient("%s","000001|û��ָ����������|");
        return 0;
     }

     //��Ͻ�����
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);

          
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|����Դ�޴˽���|");
        return 0;
     }


     Client=CreateRecord();
     if(Client.nsize<1) 
     {
     	ApplyToClient("%s","000004|����˴洢�������|");
     	return 0;
     }	
             	
     switch(nCxms)
     {
        
        //���з�ʽ
        case 110://��ͨ����
        case 610://��Ȼ������
          dsGeneralPayQuery(inbuf,&Client);
          break;
        case 111://��ͨԤ���
          break;
        
        case 130://�й��ƶ�
          dsMobilePayQuery(inbuf,&Client);
          break;

        case 140: //��������
          dsTaskTelecomPayQuery(inbuf,&Client);
          break;

        case 210://Ѱ��
        case 220:
        case 230:
          dsTaskBpUserQuery(inbuf,&Client);
          break;
        case 821:
          dsTaskBxUserQuery(inbuf,&Client);
          break;
        
        case 521://��˰����
          dsTaskTaxQuery(inbuf,&Client);
          break;
        case 522:
          dsTaskTax1Query(inbuf,&Client);
          break;
        case 123://��Ѵ���
          dsTaskPowerQuery(inbuf,&Client);
          break;
        case 141:
        //  dsTaskYaTelePayQuery(inbuf,&Client);
          break;
     }
     
     nRetVal=ApplyToClient("%r",Client);
     if(nSysDebug) printf("\nRECORD:\n%s\n",Client.sdata);
     DropRecord(&Client);
     return nRetVal;

}

/*==================================================================
������dsTaskGeneralPayQuery
���ܣ����ڼ���ʽ�շѵ�ͳһ��ѯ
*=================================================================*/

long dsGeneralPayQuery(char * inbuf,RECORD * Client)
{
     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10];
     char sYhmc[51],sYhbz1[41];//�û��������ͬ��
     char sWhere[1000];
     char sBz[31];
     char sCzydh[7];
     long nmap,nId,ncount,nysnd,nysyf,nRetVal;
     double dsfmx[26],dZnj;
     long nValue[4],nDates,nCxms;
     char * sql,*smap,* pointer;     

     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     smap=sBzmap;
     sql=sWhere;
     memset(sWhere,'\0',sizeof(sWhere));
     memset(nValue,0,sizeof(nValue));     

     strcpy(sql," where nxzbz=0 and nywlb=0 ");
     while(sql[0]) sql++;
     nmap=0;
     pointer=sYhbz;          
     //�γɲ�ѯ�������ʽ
     while(smap[0])
     {
       	nmap++;

       	if(smap[0]=='0')
        {
             smap++; 
             pointer+=20;
             continue;
        }

        memset(sBz,'\0',sizeof(sBz));
        strncpy(sBz,pointer,20);
        alltrim(sBz);
      	switch(nmap)
       	{
       	     case 1:
       	        sprintf(sql," and cyhbz='%s'",sBz);
       	        break;
       	     case 2:
       	        sprintf(sql," and cyhbz1='%s'",sBz);
       	        break;
       	     case 3:
       	        sprintf(sql," and cyhbz2='%s'",sBz);
       	        break;
       	}
        while(sql[0]) sql++;
        pointer+=20;
        smap++;
     }	

     ncount=0;
     nRetVal=RunSelect("select count(*),min(cyhmc),min(cyhbz1) \
                        from dl$ywmx_%t %t into %ld%s%s",
                        sJym,sWhere,&ncount,sYhmc,sYhbz1);

     if(nRetVal<0)
     {
         GetSqlErrInfo(sWhere);
         ImportRecord(Client,"%s%s","000011",sWhere);
         return 0;
     }

     if(ncount<=0)
     {
         ImportRecord(Client,"%s%s","000001","���û�û��Ƿ�ѻ�û�и��û���Ϣ");
         return 0;     
     }

     
     nId=OpenCursor("select cyhbz,nysnd,nysyf,dsfmx1,dsfmx2,dsfmx3,dsfmx4,\
                      dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,dsfmx11,\
                      dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,\
                      dsfmx18,dsfmx19,dsfmx20,dsfmx21,dsfmx22,dsfmx23,dsfmx24,\
                      dsfmx25,dyjzk,\
                      to_number(to_date(%ld,'YYYYMMDD')-to_date(njscz2,'YYYYMMDD')) \
                      from dl$ywmx_%t %t order by -nysnd,-nysyf",
                      nSysLkrq,sJym,sWhere);
     
     if(nId<0)
     {
     	GetSqlErrInfo(sWhere);
     	ImportRecord(Client,"%s%s","200001",sWhere);
     	return 0;
     }
     
     ImportRecord(Client,"%s%s%s%lf%ld","000000",sYhmc,sYhbz1,0.0,ncount);
     while(ncount--)
     {
     	nRetVal=FetchCursor(nId,"%s%ld%ld%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf\
     	                 %lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%ld",sYhbz,
    	                 &nysnd,&nysyf,&dsfmx[0],&dsfmx[1],&dsfmx[2],&dsfmx[3],
                         &dsfmx[4],&dsfmx[5],&dsfmx[6],
                         &dsfmx[7],&dsfmx[8],&dsfmx[9],
     	                 &dsfmx[10],&dsfmx[11],&dsfmx[12],&dsfmx[13],&dsfmx[14],
     	                 &dsfmx[15],&dsfmx[16],&dsfmx[17],&dsfmx[18],&dsfmx[19],
     	                 &dsfmx[20],&dsfmx[21],&dsfmx[22],&dsfmx[23],&dsfmx[24],
     	                 &dsfmx[25],&nDates);

        dZnj=0.0;
        switch(nCxms)   //�Ը������ͽ��׵Ĳ�ͬ����
        {
            
            case 610:   //�����ɽ�Ĵ���
            {
              double dMyZnjBl,dSyZnjBl;
              if(nDates>0)
              {                
                  RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=10 and cjym=%s \
                            into %lf",sJym,&dMyZnjBl);  //�������ɽ����
                  RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=11 and cjym=%s \
                            into %lf",sJym,&dSyZnjBl);  //�������ɽ����
                  dZnj=(dsfmx[5]*dMyZnjBl+dsfmx[2]*dSyZnjBl)*nDates;
                  if(dZnj>0.0) dsfmx[25]+=dZnj;
                  else dZnj=0.0;
                  dsfmx[6]=dZnj;
              }    
            }  
            break;      
        }//end switch
             	
     	ImportRecord(Client,"%s%ld(2)%lf[26]%ld[4]%s(4)",sYhbz,nysnd,nysyf,
     			    dsfmx,nValue,".", "." , "." , ".");

        nRetVal=RunSql("update dl$ywmx_%t set clsh=%s,cczydh=%s,dxzyjk=%lf %t \
                        and nysnd=%ld and nysyf=%ld",
                        sJym,sLsh,sCzydh,dZnj,sWhere,nysnd,nysyf);
        if(nRetVal<0) break;

     }
     CloseCursor(nId);
     if(nRetVal<0)
     {
     	GetSqlErrInfo(sWhere);
     	Client->scurrent=Client->sdata;
     	Client->nsize=0;
     	ImportRecord(Client,"%s%s","000011",sWhere);
        RollbackWork();
     }
     else CommitWork();
     return 0;
}

/*
�������ɷѲ�ѯ����˳���TASK_PUBLIC_QUERY
���ܣ������û�Ҫ���ָ������������������ѯ
      �����ض��򣬲�����ѯ�����֯��ͳһ�ĸ�ʽ���ظ��ͻ���
������inbuf:��������ַ���
      ������|�����֧|����(���ڴ��α��SQL������)|
    
���أ�>0�ɹ�
      [����RECORD]
      �ɹ���000000{���˫��������|�����������|ʮ���ִ�����}....
      ʧ�ܣ�XXXXXX|������Ϣ���ֽ���|

*/
long dsTaskPublicQuery(char * inbuf)
{
	 RECORD Client;    //���ؿͻ������ݼ�¼
     
     long nRetVal,nCxms;
     char sJym[16];//������
     char sJsdh[10];
     long nType;   //��������
     
     FetchStringToArg(inbuf,"%s|%d|",sJym,&nType);
     
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|����Դ�޴˽���|");
        return 0;
     }

     Client=CreateRecord();
     if(Client.nsize<1) 
     {
     	ApplyToClient("%s","000004|����˴洢�������|");
     	return 0;
     }	
             	
     /*switch(nType)
     {
     	case 1002:   //һ��[nCxms+n]���ڵ�����Ӧ���׵Ĳ�ѯ
     		dsTaskQuery_AirTicket(inbuf,&Client);
     		break;
		
		case 1004:
			dsTaskQuery_AirSd(inbuf,&Client);
			break;
			     	
		case 1006:
			dsTaskQuery_AirTp(inbuf,&Client);
			break;

        default:
			printf("\nδ��������֧...\n");
        	break;
     }
	*/
     nRetVal=ApplyToClient("%r",Client);
     if(nSysDebug) printf("\nRECORD:\n%s\n",Client.sdata);
     DropRecord(&Client);
     return nRetVal;
}
