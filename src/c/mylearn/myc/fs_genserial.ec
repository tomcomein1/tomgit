#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/timeb.h>

/***
**  ��������fs_genserial.ec
**  ��������������ϵͳ��ų���
**  �ڲ�������
**  ����Ա����һ��
**  ����޸����ڣ�2009-6-3
****/                          
                               
#define TRUE            0
#define FALSE           -1

#define FESP_SEMP_NUM   3
#define SEM_CONSIGNID   1 
#define SEM_TAID        2 
#define SEM_SYSFLOWNO   3

#define PERSONE         '0'
#define ORG             '1'
#define PXYM            "C0000"

static void createConsignID(int,char *,char *,char *);
static void createTAID(int,char *,char *,char *,char *,char *);
static void createFlowNo(int imachine_no, char *pcur, char *pflow_no);
static key_t getShmKey(char *);
static key_t getSemKey(char *);
static int semgetFespSem(key_t, int *);
static int getSemLock(int, int);
static int freeSemLock(int, int);

/****
 **�����Ա����һ��
 **���ܣ�����ί�����뵥���
 **�����������Ʒ���
 **���������ί�����뵥���(28λ��17λ��Ʒ��+11λ���)
 **                         27λ��19λ��Ʒ��+8λ���
 **                         26λ��18λ��Ʒ��+8λ���)
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
 **ע�⣺���붨��� SEM_CONSIGNID=1
****/
int PubGetConsignID(char *pprod_code, char *pconsignID)
{
    int     max_shm_size = 11 + 8 + 8 + 1;
    key_t   keySHM      = 0;
    key_t   keySEM      = 0;
    int     shmID       = 0;
    int     semID       = 0;
    int     iInitial    = 0;
    void    *pShmAddr;

    int     istr_len = 0;
    int     ret = FALSE;
    char    sshm_str[11 + 8 + 8 + 1];
    char    scur_17_11[11+1];
    char    scur_18_8[8+1];
    char    scur_19_8[8+1];
    char    sconsignID[28+1];

    char    *pmachine_no;
    int     imachine_no = 0;

    //��ʼ��
    memset(sshm_str, 0, sizeof(sshm_str));
    memset(scur_17_11, 0, sizeof(scur_17_11));
    memset(scur_18_8, 0, sizeof(scur_18_8));
    memset(scur_19_8, 0, sizeof(scur_19_8));
    memset(sconsignID, 0, sizeof(sconsignID));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //������
    PubStrTrim(pprod_code);
    if ((istr_len=strlen(pprod_code)) < 17 || istr_len > 19)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "����Ĳ�Ʒ���%s���Ȳ���ȷ", pprod_code);
        return FALSE;
    }
    //��ȡ�������
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "δ���廷������FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO������0��1");
        return FALSE;
    }
    //��ȡKEY
    keySHM = getShmKey("FESPCONSIGNIDSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//��ȡ�źŵ�
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //���������ڴ�
    if (getSemLock(SEM_CONSIGNID, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "����ί�����뵥��Ź����ڴ�ʧ��");
        return FALSE;
    }
    //��ȡ�����ڴ�
    if ((shmID = shmget(keySHM, max_shm_size, 0666)) < 0)
    {
        //��ȡʧ�ܵģ���ȡ���ݿ��е�ǰ���ί�����뵥���
        ret = qryMaxConsignID(scur_17_11, scur_18_8, scur_19_8);
        if (ret == FALSE)
        {
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
        //���������ڴ�
    	if ((shmID = shmget(keySHM, max_shm_size, IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "����ί�����뵥��Ź����ڴ�ʧ��");
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
        //��ʼ�������ڴ�
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ�ί�����뵥��Ź����ڴ�ʧ��");
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
        snprintf(sshm_str, 27+1, "%-11.11s%-8.8s%-8.8s", scur_17_11, scur_18_8,
                 scur_19_8);
        memset((char *)pShmAddr, 0, max_shm_size);
        memcpy((char *)pShmAddr, sshm_str, sizeof(sshm_str));
    }
    else
    {
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ�ί�����뵥��Ź����ڴ�ʧ��");
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
       memcpy(sshm_str, pShmAddr, sizeof(sshm_str));
    }

    //������ί�����뵥��š����蹲���ڴ���
    if (istr_len == 17)
    {
        snprintf(scur_17_11, 11+1, "%-.11s", sshm_str);
        createConsignID(imachine_no, pprod_code, scur_17_11, pconsignID);
        memccpy(sshm_str, pconsignID + 17, '\0', 11);
    }
    else if (istr_len == 18)
    {
        snprintf(scur_18_8, 8+1, "%-.8s", sshm_str+11);
        createConsignID(imachine_no, pprod_code, scur_18_8, pconsignID);
        memccpy(sshm_str + 11, pconsignID + 18, '\0', 8);
    }
    else
    {
        snprintf(scur_19_8, 8+1, "%-.8s", sshm_str+19);
        createConsignID(imachine_no, pprod_code, scur_19_8, pconsignID);
        memccpy(sshm_str + 19, pconsignID + 19, '\0', 8);
    }

    memcpy(pShmAddr, sshm_str, sizeof(sshm_str));
    //�Ͽ������ڴ沢����
    shmdt(pShmAddr);
    freeSemLock(SEM_CONSIGNID, semID);
    //����
    return TRUE;
}

/****
 **�����Ա����һ��
 **���ܣ�����TA���
 **���������TA�����š��ͻ�����
 **���������TA���(12λ��2λTA������+1λ�ͻ�����+8λ���+1λЧ��λ)
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
 **ע�⣺���붨��� SEM_TAID=2
****/
int PubGetTaID(char *pta_org, char *pcust_type, char *pcust_ta_id)
{
    key_t   keySHM      = 0;
    key_t   keySEM      = 0;
    int     shmID       = 0;
    int     semID       = 0;
    int     iInitial    = 0;
    void    *pShmAddr;

    int     istr_len = 0;
    int     ret = FALSE;
    char    spriv_cur[12+1];
    char    spub_cur[12+1];
    char    sshm_str[12+12+1];
    char    staID[12+1];

    char    *pmachine_no;
    int     imachine_no = 0;

    //��ʼ��
    memset(spriv_cur, 0, sizeof(spriv_cur));
    memset(spub_cur, 0, sizeof(spub_cur));
    memset(sshm_str, 0, sizeof(sshm_str));
    memset(staID, 0, sizeof(staID));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //������
    PubStrTrim(pta_org);
    if (strlen(pta_org) != 2)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "�����TA������%s���ȱ�����2λ", pta_org);
        return FALSE;
    }
    if (pcust_type[0] != PERSONE && pcust_type[0] != ORG)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "����Ŀͻ����ͱ�����0��1");
        return FALSE;
    }
    //��ȡ�������
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "δ���廷������FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO������0��1");
        return FALSE;
    }
    //��ȡKEY
    keySHM = getShmKey("FESPTAIDSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//��ȡ�źŵ�
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //���������ڴ�
    if (getSemLock(SEM_TAID, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "�����ͻ�TA��Ź����ڴ�ʧ��");
        return FALSE;
    }
    //��ȡ�����ڴ�
	if ((shmID = shmget(keySHM, sizeof(sshm_str), 0666)) < 0)
	{
        //��ȡʧ�ܵģ���ȡ���ݿ��е�ǰ���TA���
        ret = qryMaxTAID(spriv_cur, spub_cur);
        if (ret == FALSE)
        {
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        //���������ڴ�
    	if ((shmID = shmget(keySHM, sizeof(sshm_str)
            , IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "�����ͻ�TA��Ź����ڴ�ʧ��");
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        //��ʼ�������ڴ�
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ��ͻ�TA��Ź����ڴ�ʧ��");
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        snprintf(sshm_str, 12+12+1, "%-12.12s%-12.12s", spriv_cur, spub_cur);
        memset((char *)pShmAddr, 0, sizeof(sshm_str));
        memcpy((char *)pShmAddr, sshm_str, sizeof(sshm_str));
	}
    else
    {
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ��ͻ�TA��Ź����ڴ�ʧ��");
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        memcpy(sshm_str, pShmAddr, sizeof(sshm_str));
        snprintf(spriv_cur, 12+1, "%-12.12s", sshm_str);
        snprintf(spub_cur, 12+1, "%-12.12s", sshm_str+12);
    }
    //����TA��ţ������±�ŵ������ڴ�
    createTAID(imachine_no, pta_org, pcust_type, spriv_cur, spub_cur
        ,pcust_ta_id);
    snprintf(sshm_str, 12+12+1, "%-12.12s%-12.12s", spriv_cur, spub_cur);
    memcpy((char *)pShmAddr, sshm_str, sizeof(sshm_str));
    //�Ͽ������ڴ沢����
    shmdt(pShmAddr);
    freeSemLock(SEM_TAID, semID);
    //����
    return TRUE;
}

/****
 **�����Ա����һ��
 **���ܣ�������ˮ���
 **�����������
 **�����������ˮ���(17λ��1λ������+6λʱ����+3λ����+7λ���)
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
 **ע�⣺���붨��� SEM_SYSFLOWNO=3
****/
int PubGetSysFlowNo(char *pflow_no)
{
    int     max_shm_size = 19 + 1;
    key_t   keySHM      = 0;
    key_t   keySEM      = 0;
    int     shmID       = 0;
    int     semID       = 0;
    int     iInitial    = 0;
    void    *pShmAddr;

    int     ret = FALSE;
    char    scur[19+1];
    char    sflow_no[19+1];

    char    *pmachine_no;
    int     imachine_no = 0;

    //��ʼ��
    memset(scur, 0, sizeof(scur));
    memset(sflow_no, 0, sizeof(sflow_no));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //��ȡ�������
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "δ���廷������FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO������0��1");
        return FALSE;
    }
    //��ȡKEY
    keySHM = getShmKey("FESPFLOWNOSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//��ȡ�źŵ�
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //���������ڴ�
    if (getSemLock(SEM_SYSFLOWNO, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "������ˮ��Ź����ڴ�ʧ��");
        return FALSE;
    }
    //��ȡ�����ڴ�
	if ((shmID = shmget(keySHM, max_shm_size, 0666)) < 0)
	{
        //��ȡʧ�ܵģ���ȡ���ݿ��е�ǰ�����ˮ���
        ret = getMaxSysflowNo(imachine_no, scur);
        if (ret == FALSE)
        {
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        //���������ڴ�
    	if ((shmID = shmget(keySHM, max_shm_size, IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "������ˮ��Ź����ڴ�ʧ��");
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        //��ʼ�������ڴ�
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ���ˮ��Ź����ڴ�ʧ��");
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        memset((char *)pShmAddr, 0, max_shm_size);
        memcpy((char *)pShmAddr, scur, sizeof(scur));
	}
    else
    {
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "���ӵ���ˮ��Ź����ڴ�ʧ��");
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        memcpy(scur, pShmAddr, sizeof(scur));
    }
    //��������ˮ��š����蹲���ڴ���
    createFlowNo(imachine_no, scur, pflow_no);
    memccpy(scur, pflow_no, '\0', sizeof(scur));
    memccpy(pShmAddr, scur, '\0', sizeof(scur));
    //�Ͽ������ڴ沢����
    shmdt(pShmAddr);
    freeSemLock(SEM_SYSFLOWNO, semID);
    //����
    return TRUE;
}


/****
 **�����Ա����һ��
 **���ܣ�����ί�����뵥���
 **���������������š���Ʒ��š���ǰ���
 **���������ί�����뵥���(28λ��17λ��Ʒ��+11λ���)
 **                         27λ��19λ��Ʒ��+8λ���
 **                         26λ��18λ��Ʒ��+8λ���)
 **����ֵ����
 **�޸ĵ�ȫ�ֱ�����
****/
static void createConsignID(
    int     imachine_no
    ,char   *pprod_code
    ,char   *pcur
    ,char   *pconsignID
    )
{
    int     istr_len = 0;
    int     itmp = 0;
    int     imode = 0;
    double  dtmp = 0.0;
    char    stmp[20];
    char    sconsignID[28+1];

    //��ʼ��
    memset(stmp, 0, sizeof(stmp));
    memset(sconsignID, 0, sizeof(sconsignID));

    //�������
    PubStrTrim(pprod_code);
    istr_len = strlen(pprod_code);
    if (istr_len == 17)
    {
        dtmp = atof(pcur) + 1;
        if (strlen(pcur) > 9)
            itmp = atoi(pcur+2) + 1;
        else
            itmp = atoi(pcur) + 1;
        imode = itmp%2;
        if (imode == 0 && imachine_no == 1
            || imode == 1 && imachine_no == 0)
            dtmp += 1;
        snprintf(stmp, 11+1, "%011.0lf", dtmp);
        snprintf(sconsignID, 28+1, "%-s%-s", pprod_code, stmp);
    }
    else if (istr_len == 18)
    {
        itmp = atoi(pcur) + 1;
        imode = itmp%2;
        if (imode == 0 && imachine_no == 1
            || imode == 1 && imachine_no == 0)
            itmp++;
        snprintf(stmp, 8+1, "%08d", itmp);
        snprintf(sconsignID, 26+1, "%-s%-s", pprod_code, stmp);
    }
    else
    {
        itmp = atoi(pcur) + 1;
        imode = itmp%2;
        if (imode == 0 && imachine_no == 1
            || imode == 1 && imachine_no == 0)
            itmp++;
        snprintf(stmp, 8+1, "%08d", itmp);
        snprintf(sconsignID, 27+1, "%-s%-s", pprod_code, stmp);
    }

    //����
    PubStrncpy(pconsignID, sconsignID, 28);
    return;
}

/****
 **�����Ա����һ��
 **���ܣ����ɿͻ�TA���
 **��������������š�TA�����š��ͻ����͡���˽��ǰ��š��Թ���ǰ���
 **����������ͻ�TA���(12λ��2λTA������+1λ�ͻ�����+8λ���+1λЧ��λ)
 **����ֵ����
 **�޸ĵ�ȫ�ֱ�����
****/
static void createTAID(
    int     imachine_no
    ,char   *pta_org
    ,char   *pcust_type
    ,char   *ppriv_cur
    ,char   *ppub_cur
    ,char   *pcust_ta_id
    )
{
    int     itmp = 0;
    int     imode = 0;
    char    sCur[12+1];
    char    stmp[20];
    char    sTAID[28+1];
    int     ret = FALSE;

    char    scust[1+1];
    int     icust;

    //��ʼ��
    memset(stmp, 0, sizeof(stmp));
    memset(sTAID, 0, sizeof(sTAID));
    memset(scust, 0, sizeof(scust));

    //���ֶ�˽���Թ��ͻ���ȡ��ǰ������
    if (pcust_type[0] == PERSONE)
        PubStrncpy(sCur, ppriv_cur, 12);
    else
        PubStrncpy(sCur, ppub_cur, 12);
    //��ȡ�ͻ��������
    PubStrncpy(scust, sCur+2, 1);
    icust = atoi(scust);
    //��ȡ8λ��Ų�����
    snprintf(stmp, 8+1, "%-8.8s", sCur+3);
    itmp = atoi(stmp);
    if (imachine_no == 0 && itmp >= 99999998
        || imachine_no == 1 && itmp >= 99999999)
    {
        itmp = 1;
        icust++;
        if (pcust_type[0] == PERSONE && icust >= 6)
            icust = 1;
        else if (pcust_type[0] == ORG && icust >= 9)
            icust = 6;
    }
    else
        itmp++;
    //���ֻ���������ż���
    imode = itmp%2;
    if (imode == 0 && imachine_no == 1
        || imode == 1 && imachine_no == 0)
        itmp++;
    snprintf(sTAID, 12+1, "%-2.2s%1d%08d0", pta_org, icust, itmp);
    //���ɿͻ�TA���Ч��λ
    PubChkVerify(sTAID, 1);
    //����
    if (pcust_type[0] == PERSONE)
        PubStrncpy(ppriv_cur, sTAID, 12);
    else
        PubStrncpy(ppub_cur, sTAID, 12);
    PubStrncpy(pcust_ta_id, sTAID, 12);
    return;
}

/****
 **�����Ա����һ��
 **���ܣ�������ˮ���
 **���������������š���ǰ�ڴ���ˮ��
 **�����������ˮ���(17λ��1λ������+6λʱ����+3λ����+7λ���)
 **�����������ˮ���(19λ��1λ������+6λ������+6λʱ����+3λ����+3λ���)
 **����ֵ����
 **�޸ĵ�ȫ�ֱ�����
****/
static void createFlowNo(int imachine_no, char *pcur, char *pflow_no)
{
    int     itmp = 0;
    int     imode = 0;
    char    stmp[3+1];
    char    sflow_no[19+1];
    struct  timeb sttimeb;
	struct  tm *local;

    //��ʼ��
    memset(stmp, 0, sizeof(stmp));
    memset(sflow_no, 0, sizeof(sflow_no));
    memset(&sttimeb, 0, sizeof(sttimeb));

    //��ȡ��ǰʱ��
    ftime(&sttimeb);
	local = localtime(&sttimeb.time);

    //������ˮ��
    itmp = 1;
    snprintf(sflow_no, 19+1, "%01d%02d%02d%02d%02d%02d%02d%03d%03d"
        ,imachine_no
        ,local->tm_year%100, local->tm_mon+1, local->tm_mday
        ,local->tm_hour, local->tm_min, local->tm_sec
        , sttimeb.millitm, itmp);
    if (memcmp(sflow_no, pcur, 16) == 0)
    {
        memccpy(stmp, pcur+16, '\0', 3);
        stmp[3] = '\0';
        itmp = atoi(stmp) + 1;
        snprintf(stmp, 3+1, "%03d", itmp);
        memcpy(sflow_no+16, stmp, 3);
    }
    //����
    PubStrncpy(pflow_no, sflow_no, 19);
    return;
}

/****
 **�����Ա����һ��
 **���ܣ������ļ�����ȡ�����ڴ�KEY
 **����������ļ���
 **�����������
 **����ֵ�������ڴ�KEY
 **�޸ĵ�ȫ�ֱ�����
****/
static key_t getShmKey(char *filename)
{
    char    full_filename[128 + 1];
    key_t   keySHM;

    snprintf(full_filename, 128+1, "%s/etc/%s", getenv( "HOME" ), filename);
    keySHM = ftok(full_filename, 1);
    if (keySHM == -1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "��ȡ%sָ����KEY����", filename);
        return -1;
    }
    //����
    return keySHM;
}

/****
 **�����Ա����һ��
 **���ܣ������ļ�����ȡ�źŵ�KEY
 **����������ļ���
 **�����������
 **����ֵ���źŵ�KEY
 **�޸ĵ�ȫ�ֱ�����
****/
static key_t getSemKey(char *filename)
{
    char    full_filename[128 + 1];
    key_t   keySEM;

    snprintf(full_filename, 128+1, "%s/etc/%s", getenv( "HOME" ), filename);
    keySEM = ftok(full_filename, 'S');
    if (keySEM == -1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "��ȡ%sָ����SEM KEY����", filename);
        return -1;
    }
    //����
    return keySEM;
}

/****
 **�����Ա����һ��
 **���ܣ�����ϵͳ�źŵ�
 **����������źŵ�KEY
 **����������źŵ�ID
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
****/
static int semgetFespSem(key_t keySEM, int *psem_id)
{
    int     semID = 0;
    int     iInitial = 0;

	if ((semID = semget(keySEM, FESP_SEMP_NUM, 0666)) < 0)
	{
        if ((semID = semget(keySEM, FESP_SEMP_NUM, IPC_CREAT|IPC_EXCL|0666))
            < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "�����źŵ�ʧ��");
            return FALSE;
        }
        //��ʼ���źŵ�
        iInitial = 0;
        if (semctl(semID, FESP_SEMP_NUM, SETALL, iInitial) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "��ʼ���źŵ�ʧ��");
            return FALSE;
        }
	}
    //����
    *psem_id = semID;
    return TRUE;
}

/****
 **�����Ա����һ��
 **���ܣ���ȡ�źŵ���
 **����������źŵ�����λ�á��źŵ�KEY
 **�����������
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
****/
static int getSemLock(int isemnum, int isemID)
{
	int     i = 0;
    int     ret = FALSE;

	static struct sembuf stSem_Lock[2]=
	{
		0,0,IPC_NOWAIT|SEM_UNDO,    //�ȴ��ź���ֵΪ0
		0,1,IPC_NOWAIT|SEM_UNDO	    //���ź�������
	};

    if (isemnum < 0 || isemnum > FESP_SEMP_NUM)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "Ҫ��ȡ���źŵ�λ��%d������Χ0-%d", isemnum, FESP_SEMP_NUM);
        return FALSE;
    }

    stSem_Lock[0].sem_num = isemnum - 1;
    stSem_Lock[1].sem_num = isemnum - 1;

    ret = FALSE;
	for (i=0; i<20; i++)
	{
		if (semop(isemID, &stSem_Lock[0], 2) < 0)
            usleep(500 + i * 500);
		else
        {
            ret = TRUE;
            break;
        }
	}
    if (ret == FALSE)
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "��ȡ%d�źŵ�ʧ�ܡ�errno=%d", isemID, errno);
	return (ret);
}


/****
 **�����Ա����һ��
 **���ܣ�����źŵ���
 **����������źŵ�����λ�á��źŵ�KEY
 **�����������
 **����ֵ��TRUE/FALSE
 **�޸ĵ�ȫ�ֱ�����
****/
static int freeSemLock(int isemnum, int isemID)
{
	static struct sembuf stSem_UnLock[1]=
	{
		0, -1, (IPC_NOWAIT|SEM_UNDO)
	};

    if (isemnum < 0 || isemnum > FESP_SEMP_NUM)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "Ҫ�ͷŵ��źŵ�λ��%d������Χ0-%d", isemnum, FESP_SEMP_NUM);
        return FALSE;
    }

    stSem_UnLock[0].sem_num = isemnum - 1;

    if ((semop(isemID, &stSem_UnLock[0], 1)) < 0)
        return FALSE;
    else
        return TRUE;
}



