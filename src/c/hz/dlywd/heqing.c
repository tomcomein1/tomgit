long nQdSpbh(char *sSpmc);

long nQdSpbh(char *sSpmc)
{
	long nId;
	long nId1;
	long nMaxYwbh;
	
	nId=RunSelect("select max(nbh) from dl$ywzl into %ld",&nMaxYwbh);
	if(nId<0)
	{
		/*ͳһ��������*/
        //         HintError(DB_ERR,"��ѯҵ��������ų���!!!");
       	         return ApplyToClient("%ld",-1);
	}
	nMaxYwbh=nMaxYwbh+1;
	nId1=RunSql("insert into dl$ywzl value(%ld,%s,0,'0','2000000000','0')",nMaxYwbh,sSpmc);
	if(nId<0)
	{
		
		/*ͳһ��������*/
        //         HintError(DB_ERR,"ҵ�������������!!!");
	         return ApplyToClient("%ld",-1);
	}
        printf("\n the retval=%ld",nMaxYwbh);
	return ApplyToClient("%ld",nMaxYwbh);
}

