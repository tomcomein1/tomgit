/*==============================================================
           setupini.c:��װģ��[���ϳ�ʼ��]
           
����Ա���²�

����       �޸���      ԭ��

*==============================================================*/

#include<stdio.h>

extern char sSysJh[10];
extern char sSysJm[21];
extern char sSysIp[16];

void InitDljh()
{
    outtext("\n��ʼ�������Ĵ�ʡ�ֺű�...\n");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270000000','ʡ��Ϣ����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270100000','�ɶ�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270200000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270300000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270400000','�˱�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270500000','�ڽ�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270600000','�Թ�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270700000','�㰲','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270800000','�ϳ�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270900000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271000000','��Ԫ','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271100000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271200000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271300000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271400000','����','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271500000','üɽ','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271600000','�Ű�','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271700000','��ɽ','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271800000','������','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271900000','��ɽ��','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272000000','������','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272100000','��֦��','.',2,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272200000','����Ʊ��','.',2,'0000000000','.')");
    
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270001000','ʡ��Ϣ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270001001','ʡ��Ϣ��������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270101000','�ɶ�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270101001','�ɶ���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270110000','��׽�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270110001','��׽���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270201000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270201001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270202000','��̨','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270202001','��̨��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270203000','��ͤ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270204000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270204001','���ʹ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270205000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270205001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270206000','ƽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270206001','ƽ���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270207000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270207001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270208000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270208001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270301000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270301001','���ݴ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270304000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270304001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270303000','�Ͻ�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270303001','�Ͻ���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270302000','��Ϫ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270302001','��Ϫ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270306000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270306001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270305000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270305001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270401000','�˱�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270401001','�˱���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270402000','��Ϫ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270402001','��Ϫ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270403000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270403001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270404000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270404001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270405000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270405001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270406000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270406001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270407000','�˱���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270407001','�˱��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270408000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270409000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270410000','��ɽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270410001','��ɽ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270501000','�ڽ�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270501001','�ڽ���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270502000','�ڽ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270502001','�ڽ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270503000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270503001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270504000','¡����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270504001','¡���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270505000','��Զ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270505001','��Զ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270506000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270506001','��������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270601000','�Թ�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270601001','�Թ���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270602000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270602001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270603000','��˳��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270603001','��˳�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270701000','�㰲','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270701001','�㰲��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270702000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270702001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270703000','��ʤ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270703001','��ʤ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270704000','��Ө��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270704001','��Ө�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270705000','��ˮ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270705001','��ˮ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270801000','�ϳ�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270801001','�ϳ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270802000','�ϳ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270802001','�ϳ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270803000','�ϲ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270803001','�ϲ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270804000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270804001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270805000','��¤��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270805001','��¤�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270806000','Ӫɽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270806001','Ӫɽ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270807000','���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270807001','��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270808000','��ƺ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270808001','��ƺ����������)','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270809000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270809001','�����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270810000','˳����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270810001','˳������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270811000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270811001','��������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270901000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270901001','���д�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270902000','ƽ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270902001','ƽ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270903000','�Ͻ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270903001','�Ͻ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270904000','ͨ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('270904001','ͨ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271001000','��Ԫ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271001001','��Ԫ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271002000','�ന��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271002001','�ന�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271003000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271003001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271004000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271004001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271005000','��Ϫ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271005001','��Ϫ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271101000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271101001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271102000','�н�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271102001','�н���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271103000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271103001','�����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271104000','�㺺','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271104001','�㺺��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271105000','ʲ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271105001','ʲ����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271106000','�޽�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271106001','�޽���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271201000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271201001','���ݴ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271202000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271202001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271203000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271203001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271204000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271204001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271205000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271205001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271206000','��Դ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271206001','��Դ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271301000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271301001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271302000','��Ϫ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271302001','��Ϫ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271303000','�����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271303001','����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271304000','��Ӣ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271401000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271401001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271402000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271402001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271403000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271403001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271404000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271404001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271501000','üɽ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271501001','üɽ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271502000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271502001','���ٴ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271503000','��ɽ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271503001','��ɽ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271504000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271504001','���Ŵ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271505000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271505001','�����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271506000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271506001','�����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271601000','�Ű�','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271601001','�Ű���������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271602000','��ɽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271602001','��ɽ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271603000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271603001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271604000','��Դ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271604001','��Դ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271605000','ʯ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271605001','ʯ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271606000','��ȫ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271606001','��ȫ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271607000','«ɽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271607001','«ɽ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271608000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271608001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271701000','��ɽ','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271701001','��ɽ��������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271702000','�н���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271702001','�н��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271703000','��üɽ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271703001','��üɽ�д�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271704000','�������������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271704001','��ߴ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271705000','��Ϊ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271705001','��Ϊ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271706000','�崨��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271706001','�崨�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271707000','�������������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271707001','��ߴ�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271708000','��ں���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271708001','��ں�����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271709000','��ͨ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271709001','��ͨ������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271710000','ɳ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271710001','ɳ������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271711000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271711001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271801000','�����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271801001','�������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271802000','�봨��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271802001','�봨�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271803000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271803001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271804000','ï��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271804001','ï�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271805000','�ɷ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271805001','�ɷ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271806000','��ƺ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271806001','��ƺ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271807000','��ˮ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271807001','��ˮ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271808000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271808001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271809000','С����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271809001','С���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271810000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271810001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271811000','��ԭ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271811001','��ԭ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271812000','��������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271812001','�������ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271813000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271813001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271901000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271901001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271902000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271902001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271903000','�ᶫ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271903001','�ᶫ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271904000','�ո���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271904001','�ո��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271905000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271905001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271906000','�²���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271906001','�²��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271907000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271907001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271908000','��Դ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271908001','��Դ�ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271909000','ľ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271909001','ľ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271910000','�Ѿ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271910001','�Ѿ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271911000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271911001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271912000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271912001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271913000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271913001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271914000','�ײ���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271914001','�ײ��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271915000','Խ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271915001','Խ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271916000','ϲ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271916001','ϲ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271917000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('271917001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272001000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272001001','������������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272002000','����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272002001','���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272003000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272003001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272004000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272004001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272005000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272005001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272006000','¯����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272006001','¯���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272007000','ɫ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272007001','ɫ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272008000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272008001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272009000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272009001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272010000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272010001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272011000','�¸���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272011001','�¸��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272012000','ʯ����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272012001','ʯ���ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272013000','�Ž���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272013001','�Ž��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272014000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272014001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272015000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272015001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272016000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272016001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272017000','�����','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272017001','����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272018000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272018001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272101000','��֦��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272101001','��֦����������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272102000','�α���','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272102001','�α��ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272103000','������','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272103001','�����ش�������','.',4,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272201000','����Ʊ��','.',3,'0000000000','.')");
    RunSql("INSERT INTO dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) VALUES('272201001','�ɶ�Ʊ������','.',3,'0000000000','.')");
    CommitWork();
}


void InitDlcs()
{

  long nLoop;
  outtext("\n��ʼ��ϵͳ������...\n");

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
     VALUES (1,'�ֻ���','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (2,'BP����','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (3,'�̶��绰��','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (4,'ˮ��','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (5,'����','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (6,'���','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (7,'�ֻ���Ԥ��','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (8,'��Ʊ','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (9,'����','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
     VALUES (10,'��˰','0',0,0,'00000000000000000000')");

  
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (1,'��ͨ','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (2,'����Ѱ��','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (3,'�й��ƶ�','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (4,'�й�����','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (5,'��Ȼ����˾','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (6,'������˾','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (7,'����ˮ��˾','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (8,'���չ�˾','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (9,'�й����ٱ��չ�˾','0',0,0,'00000000000000000000')");
  RunSql("INSERT INTO dl$gs(ngsbh,cgsmc,cbl,nbl,dbl,clxbz) \
          VALUES (10,'��˰��','0',0,0,'00000000000000000000')");

  CommitWork();

}




