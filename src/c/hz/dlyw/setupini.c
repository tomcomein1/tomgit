/*==============================================================
           setupini.c:安装模块[资料初始化]
           
程序员：陈博

日期       修改人      原因

*==============================================================*/

#include<stdio.h>

extern char sSysJh[10];
extern char sSysJm[21];
extern char sSysIp[16];

void InitDljh()
{
    outtext("\n初始化插入四川省局号表...\n");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270000000','省信息中心','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270100000','成都','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270200000','绵阳','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270300000','泸洲','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270400000','宜宾','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270500000','内江','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270600000','自贡','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270700000','广安','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270800000','南充','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270900000','巴中','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271000000','广元','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271100000','德阳','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271200000','达州','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271300000','遂宁','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271400000','资中','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271500000','眉山','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271600000','雅安','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271700000','乐山','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271800000','阿坝州','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271900000','凉山州','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272000000','甘孜州','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272100000','攀枝花','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272200000','邮政票务','.',2,'0000000000','.')");
    
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270001000','省信息中心','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270001001','省信息代理中心','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270101000','成都','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270101001','成都代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270110000','青白江','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270110001','青白江代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270201000','绵阳','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270201001','绵阳代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270202000','三台','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270202001','三台代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270203000','盐亭','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270204000','江油','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270204001','江油代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270205000','梓潼','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270205001','梓潼代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270206000','平武','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270206001','平武代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270207000','安县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270207001','安县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270208000','北川','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270208001','北川代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270301000','泸州','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270301001','泸州代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270304000','泸县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270304001','泸县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270303000','合江','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270303001','合江代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270302000','纳溪','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270302001','纳溪代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270306000','叙永','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270306001','叙永代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270305000','古蔺','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270305001','古蔺代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270401000','宜宾','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270401001','宜宾代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270402000','南溪县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270402001','南溪县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270403000','江安县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270403001','江安县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270404000','长宁县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270404001','长宁县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270405000','兴文县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270405001','兴文县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270406000','珙县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270406001','珙县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270407000','宜宾县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270407001','宜宾县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270408000','高县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270409000','筠连县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270410000','屏山县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270410001','屏山县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270501000','内江','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270501001','内江代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270502000','内江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270502001','内江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270503000','资中县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270503001','资中县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270504000','隆昌县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270504001','隆昌县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270505000','威远县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270505001','威远县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270506000','东兴区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270506001','东兴区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270601000','自贡','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270601001','自贡代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270602000','荣县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270602001','荣县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270603000','富顺县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270603001','富顺县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270701000','广安','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270701001','广安代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270702000','岳池县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270702001','岳池县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270703000','武胜县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270703001','武胜县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270704000','华莹县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270704001','华莹县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270705000','邻水县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270705001','邻水县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270801000','南充','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270801001','南充代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270802000','南充县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270802001','南充县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270803000','南部县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270803001','南部县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270804000','阆中县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270804001','阆中县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270805000','仪陇县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270805001','仪陇县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270806000','营山县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270806001','营山县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270807000','蓬安县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270807001','蓬安县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270808000','高坪区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270808001','高坪区代理中心)','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270809000','西充','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270809001','西充代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270810000','顺庆区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270810001','顺庆区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270811000','嘉陵区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270811001','嘉陵区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270901000','巴中','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270901001','巴中代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270902000','平昌县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270902001','平昌县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270903000','南江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270903001','南江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270904000','通江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270904001','通江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271001000','广元','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271001001','广元代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271002000','青川县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271002001','青川县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271003000','旺苍县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271003001','旺苍县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271004000','剑阁县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271004001','剑阁县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271005000','苍溪县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271005001','苍溪县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271101000','德阳','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271101001','德阳代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271102000','中江','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271102001','中江代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271103000','绵竹','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271103001','绵竹代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271104000','广汉','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271104001','广汉代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271105000','什邡','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271105001','什邡代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271106000','罗江','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271106001','罗江代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271201000','达州','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271201001','达州代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271202000','大竹县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271202001','大竹县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271203000','渠县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271203001','渠县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271204000','宣汉县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271204001','宣汉县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271205000','开江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271205001','开江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271206000','万源县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271206001','万源县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271301000','遂宁','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271301001','遂宁代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271302000','蓬溪县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271302001','蓬溪县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271303000','射洪县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271303001','射洪县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271304000','大英县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271401000','资阳','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271401001','资阳代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271402000','简阳县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271402001','简阳县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271403000','乐至县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271403001','乐至县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271404000','安岳县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271404001','安岳县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271501000','眉山','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271501001','眉山代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271502000','仁寿','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271502001','仁寿代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271503000','彭山','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271503001','彭山代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271504000','洪雅','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271504001','洪雅代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271505000','丹棱','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271505001','丹棱代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271506000','青神','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271506001','青神代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271601000','雅安','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271601001','雅安代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271602000','名山县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271602001','名山县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271603000','荥经县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271603001','荥经县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271604000','汉源县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271604001','汉源县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271605000','石棉县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271605001','石棉县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271606000','天全县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271606001','天全县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271607000','芦山县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271607001','芦山县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271608000','宝兴县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271608001','宝兴县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271701000','乐山','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271701001','乐山代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271702000','夹江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271702001','夹江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271703000','峨眉山市','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271703001','峨眉山市代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271704000','峨边彝族自治县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271704001','峨边代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271705000','犍为县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271705001','犍为县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271706000','沐川县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271706001','沐川县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271707000','马边彝族自治县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271707001','马边代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271708000','金口河区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271708001','金口河区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271709000','五通桥区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271709001','五通桥区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271710000','沙湾区','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271710001','沙湾区代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271711000','井研县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271711001','井研县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271801000','马尔康','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271801001','马尔康代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271802000','汶川县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271802001','汶川县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271803000','理县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271803001','理县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271804000','茂县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271804001','茂县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271805000','松藩县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271805001','松藩县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271806000','南坪县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271806001','南坪县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271807000','黑水县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271807001','黑水县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271808000','金川县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271808001','金川县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271809000','小金县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271809001','小金县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271810000','壤塘县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271810001','壤塘县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271811000','红原县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271811001','红原县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271812000','若尔盖县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271812001','若尔盖县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271813000','阿坝县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271813001','阿坝县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271901000','西昌','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271901001','西昌代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271902000','会里县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271902001','会里县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271903000','会东县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271903001','会东县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271904000','普格县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271904001','普格县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271905000','宁南县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271905001','宁南县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271906000','德昌县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271906001','德昌县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271907000','冕宁县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271907001','冕宁县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271908000','盐源县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271908001','盐源县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271909000','木里县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271909001','木里县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271910000','昭觉县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271910001','昭觉县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271911000','金阳县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271911001','金阳县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271912000','布拖县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271912001','布拖县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271913000','美姑县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271913001','美姑县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271914000','雷波县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271914001','雷波县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271915000','越西县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271915001','越西县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271916000','喜德县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271916001','喜德县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271917000','甘洛县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271917001','甘洛县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272001000','康定','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272001001','康定代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272002000','泸定县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272002001','泸定县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272003000','九龙县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272003001','九龙县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272004000','丹巴县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272004001','丹巴县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272005000','道孚县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272005001','道孚县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272006000','炉霍县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272006001','炉霍县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272007000','色达县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272007001','色达县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272008000','甘孜县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272008001','甘孜县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272009000','新龙县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272009001','新龙县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272010000','白玉县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272010001','白玉县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272011000','德格县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272011001','德格县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272012000','石渠县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272012001','石渠县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272013000','雅江县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272013001','雅江县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272014000','理塘县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272014001','理塘县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272015000','巴塘县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272015001','巴塘县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272016000','稻城县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272016001','稻城县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272017000','乡城县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272017001','乡城县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272018000','得荣县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272018001','得荣县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272101000','攀枝花','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272101001','攀枝花代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272102000','盐边县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272102001','盐边县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272103000','米易县','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272103001','米易县代理中心','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272201000','邮政票务','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272201001','成都票务中心','.',3,'0000000000','.')");
    CommitWork();
}


void InitDlcs()
{

  long nLoop;
  outtext("\n初始化系统参数表...\n");

  alltrim(sSysIp);
  for(nLoop=1;nLoop<=100;nLoop++)
      RunSql("INSERT INTO dl$dlywcs VALUES(%ld,'0')",nLoop);

  RunSql("update dl$dlywcs set ccs=%s where nbh=1",sSysJh);
  RunSql("update dl$dlywcs set ccs=%s where nbh=2",sSysJm);
  RunSql("update dl$dlywcs set ccs=%s where nbh=3",sSysIp);
  RunSql("update dl$dlywcs set ccs='1' where nbh=17");
  RunSql("update dl$dlywcs set ccs='3' where nbh=18");
  RunSql("update dl$dlywcs set ccs='1' where nbh=19");
  RunSql("update dl$dlywcs set ccs='2' where nbh=31");
  RunSql("update dl$dlywcs set ccs='370' where nbh=32");
  RunSql("update dl$dlywcs set ccs='0101'where nbh=34");

  RunSql("insert into dl$route(sdestination,sgateway) values('dldj','localhost')");
  RunSql("insert into dl$route(sdestination,sgateway) values(%s,'localhost')",sSysIp);
  RunSql("insert into dl$route(sdestination,sgateway) values('default','dlsj')");


  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (1,'手机费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (2,'BP机费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (3,'固定电话费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (4,'水费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (5,'气费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (6,'电费','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (7,'手机费预存','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (8,'机票','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (9,'保险','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (10,'国税','0',0,0,'00000000000000000000')");

  
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (1,'联通','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (2,'国信寻呼','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (3,'中国移动','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (4,'中国电信','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (5,'天然气公司','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (6,'电力公司','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (7,'自来水公司','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (8,'航空公司','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (9,'中国人寿保险公司','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (10,'国税局','0',0,0,'00000000000000000000')");

  CommitWork();

}




