#include <stdio.h>

main()
{
   long nRetVal,nId,nval;
   double dval;
   char value[21]="\0";
   char va1[81],va2[21];
   
   
   nRetVal = ConnectDatabase("DLZ","DDDD");

   nId = OpenCursor("select cjh,cjm,nlb,0.0 from dl$jh where nlb>%ld order by cjh",1);
   for(;;)
   {
   	nRetVal=FetchCursor(nId,"%s%s%ld%lf",va1,va2,&nval,&dval);
   	
   	printf("\n%ld,%s,%s,%ld,%.2lf",nRetVal,va1,va2,nval,dval);
   	if(nRetVal<=0) break;
   }	
   CloseCursor(nId);
   CloseDatabase();

}

