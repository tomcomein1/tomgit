
void dsUnicomFileTrans()
{
  
   long nYckFlag,nDsfFLag;
   char sRemoteDir[51],sRemoteUser[21],sRemotePass[21];
   char sLocaDir[41];
   long nLastDsfDate,  //������ʹ��շ���ĩ�ļ�������
        nLastYckDate,  //
        nLastTime,
        nLastYmDate;

   if(DlsrvConnectDatabase()<0)
   {
   	printf("\ncant't connect database as user dlz.");
   	exit(0);
   }
   
   nYckFlag=RunSelect("select * from dl$jym where ncxms=111");
   nDsfFlag=RunSelect("select * from dl$jym where ncxms=110");
   
   

   




}

