long nQdSpbh(char *sSpmc);

long nQdSpbh(char *sSpmc)
{
	long nId;
	long nId1;
	long nMaxYwbh;
	
	nId=RunSelect("select max(nbh) from dl$ywzl into %ld",&nMaxYwbh);
	if(nId<0)
	{
		/*统一报错函数的*/
        //         HintError(DB_ERR,"查询业务种类表编号出错!!!");
       	         return ApplyToClient("%ld",-1);
	}
	nMaxYwbh=nMaxYwbh+1;
	nId1=RunSql("insert into dl$ywzl value(%ld,%s,0,'0','2000000000','0')",nMaxYwbh,sSpmc);
	if(nId<0)
	{
		
		/*统一报错函数的*/
        //         HintError(DB_ERR,"业务种类表入库出错!!!");
	         return ApplyToClient("%ld",-1);
	}
        printf("\n the retval=%ld",nMaxYwbh);
	return ApplyToClient("%ld",nMaxYwbh);
}

