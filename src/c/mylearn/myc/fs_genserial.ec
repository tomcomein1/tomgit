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
**  程序名：fs_genserial.ec
**  功能描述：生成系统序号程序
**  内部函数：
**  程序员：滕一勤
**  最后修改日期：2009-6-3
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
 **编程人员：滕一勤
 **功能：生成委托申请单编号
 **输入参数：产品编号
 **输出参数：委托申请单编号(28位：17位产品号+11位序号)
 **                         27位：19位产品号+8位序号
 **                         26位：18位产品号+8位序号)
 **返回值：TRUE/FALSE
 **修改的全局变量：
 **注意：必须定义宏 SEM_CONSIGNID=1
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

    //初始化
    memset(sshm_str, 0, sizeof(sshm_str));
    memset(scur_17_11, 0, sizeof(scur_17_11));
    memset(scur_18_8, 0, sizeof(scur_18_8));
    memset(scur_19_8, 0, sizeof(scur_19_8));
    memset(sconsignID, 0, sizeof(sconsignID));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //检查参数
    PubStrTrim(pprod_code);
    if ((istr_len=strlen(pprod_code)) < 17 || istr_len > 19)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "输入的产品编号%s长度不正确", pprod_code);
        return FALSE;
    }
    //获取机器编号
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "未定义环境变量FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO仅允许0、1");
        return FALSE;
    }
    //获取KEY
    keySHM = getShmKey("FESPCONSIGNIDSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//获取信号灯
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //锁定共享内存
    if (getSemLock(SEM_CONSIGNID, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "锁定委托申请单序号共享内存失败");
        return FALSE;
    }
    //获取共享内存
    if ((shmID = shmget(keySHM, max_shm_size, 0666)) < 0)
    {
        //获取失败的，获取数据库中当前最大委托申请单编号
        ret = qryMaxConsignID(scur_17_11, scur_18_8, scur_19_8);
        if (ret == FALSE)
        {
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
        //创建共享内存
    	if ((shmID = shmget(keySHM, max_shm_size, IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "创建委托申请单序号共享内存失败");
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
        //初始化共享内存
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "连接到委托申请单序号共享内存失败");
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
                , "连接到委托申请单序号共享内存失败");
            freeSemLock(SEM_CONSIGNID, semID);
            return FALSE;
        }
       memcpy(sshm_str, pShmAddr, sizeof(sshm_str));
    }

    //生成新委托申请单编号、重设共享内存编号
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
    //断开共享内存并解锁
    shmdt(pShmAddr);
    freeSemLock(SEM_CONSIGNID, semID);
    //返回
    return TRUE;
}

/****
 **编程人员：滕一勤
 **功能：生成TA编号
 **输入参数：TA机构号、客户类型
 **输出参数：TA编号(12位：2位TA机构号+1位客户类型+8位序号+1位效验位)
 **返回值：TRUE/FALSE
 **修改的全局变量：
 **注意：必须定义宏 SEM_TAID=2
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

    //初始化
    memset(spriv_cur, 0, sizeof(spriv_cur));
    memset(spub_cur, 0, sizeof(spub_cur));
    memset(sshm_str, 0, sizeof(sshm_str));
    memset(staID, 0, sizeof(staID));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //检查参数
    PubStrTrim(pta_org);
    if (strlen(pta_org) != 2)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "输入的TA机构号%s长度必须是2位", pta_org);
        return FALSE;
    }
    if (pcust_type[0] != PERSONE && pcust_type[0] != ORG)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "输入的客户类型必须是0、1");
        return FALSE;
    }
    //获取机器编号
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "未定义环境变量FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO仅允许0、1");
        return FALSE;
    }
    //获取KEY
    keySHM = getShmKey("FESPTAIDSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//获取信号灯
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //锁定共享内存
    if (getSemLock(SEM_TAID, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "锁定客户TA编号共享内存失败");
        return FALSE;
    }
    //获取共享内存
	if ((shmID = shmget(keySHM, sizeof(sshm_str), 0666)) < 0)
	{
        //获取失败的，获取数据库中当前最大TA编号
        ret = qryMaxTAID(spriv_cur, spub_cur);
        if (ret == FALSE)
        {
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        //创建共享内存
    	if ((shmID = shmget(keySHM, sizeof(sshm_str)
            , IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "创建客户TA编号共享内存失败");
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        //初始化共享内存
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "连接到客户TA编号共享内存失败");
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
                , "连接到客户TA编号共享内存失败");
            freeSemLock(SEM_TAID, semID);
            return FALSE;
        }
        memcpy(sshm_str, pShmAddr, sizeof(sshm_str));
        snprintf(spriv_cur, 12+1, "%-12.12s", sshm_str);
        snprintf(spub_cur, 12+1, "%-12.12s", sshm_str+12);
    }
    //生成TA编号，放置新编号到共享内存
    createTAID(imachine_no, pta_org, pcust_type, spriv_cur, spub_cur
        ,pcust_ta_id);
    snprintf(sshm_str, 12+12+1, "%-12.12s%-12.12s", spriv_cur, spub_cur);
    memcpy((char *)pShmAddr, sshm_str, sizeof(sshm_str));
    //断开共享内存并解锁
    shmdt(pShmAddr);
    freeSemLock(SEM_TAID, semID);
    //返回
    return TRUE;
}

/****
 **编程人员：滕一勤
 **功能：生成流水编号
 **输入参数：无
 **输出参数：流水编号(17位：1位机器号+6位时分秒+3位毫秒+7位序号)
 **返回值：TRUE/FALSE
 **修改的全局变量：
 **注意：必须定义宏 SEM_SYSFLOWNO=3
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

    //初始化
    memset(scur, 0, sizeof(scur));
    memset(sflow_no, 0, sizeof(sflow_no));

    pShmAddr    = NULL;
    pmachine_no = NULL;

    //获取机器编号
    if ((pmachine_no = getenv("FESP_MACHINE_NO")) == NULL)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "未定义环境变量FESP_MACHINE_NO");
        return FALSE;
    }
    imachine_no = atoi(pmachine_no);
    if (imachine_no != 0 && imachine_no != 1)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "FESP_MACHINE_NO仅允许0、1");
        return FALSE;
    }
    //获取KEY
    keySHM = getShmKey("FESPFLOWNOSHMKEY");
    if (keySHM == -1)
        return FALSE;
    keySEM = getSemKey("FESPSEMKEY");
    if (keySEM == -1)
        return FALSE;
	//获取信号灯
    ret = semgetFespSem(keySEM, &semID);
    if (ret == FALSE)
        return FALSE;
    //锁定共享内存
    if (getSemLock(SEM_SYSFLOWNO, semID) == FALSE)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "锁定流水序号共享内存失败");
        return FALSE;
    }
    //获取共享内存
	if ((shmID = shmget(keySHM, max_shm_size, 0666)) < 0)
	{
        //获取失败的，获取数据库中当前最大流水序号
        ret = getMaxSysflowNo(imachine_no, scur);
        if (ret == FALSE)
        {
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        //创建共享内存
    	if ((shmID = shmget(keySHM, max_shm_size, IPC_CREAT|IPC_EXCL|0666)) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "创建流水序号共享内存失败");
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        //初始化共享内存
        pShmAddr = (void *)shmat(shmID, 0, 0);
        if ((int)pShmAddr == -1)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "连接到流水序号共享内存失败");
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
                , "连接到流水序号共享内存失败");
            freeSemLock(SEM_SYSFLOWNO, semID);
            return FALSE;
        }
        memcpy(scur, pShmAddr, sizeof(scur));
    }
    //生成新流水序号、重设共享内存编号
    createFlowNo(imachine_no, scur, pflow_no);
    memccpy(scur, pflow_no, '\0', sizeof(scur));
    memccpy(pShmAddr, scur, '\0', sizeof(scur));
    //断开共享内存并解锁
    shmdt(pShmAddr);
    freeSemLock(SEM_SYSFLOWNO, semID);
    //返回
    return TRUE;
}


/****
 **编程人员：滕一勤
 **功能：生成委托申请单编号
 **输入参数：机器编号、产品编号、当前序号
 **输出参数：委托申请单编号(28位：17位产品号+11位序号)
 **                         27位：19位产品号+8位序号
 **                         26位：18位产品号+8位序号)
 **返回值：无
 **修改的全局变量：
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

    //初始化
    memset(stmp, 0, sizeof(stmp));
    memset(sconsignID, 0, sizeof(sconsignID));

    //生成序号
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

    //返回
    PubStrncpy(pconsignID, sconsignID, 28);
    return;
}

/****
 **编程人员：滕一勤
 **功能：生成客户TA编号
 **输入参数：机器号、TA机构号、客户类型、对私当前序号、对公当前序号
 **输出参数：客户TA编号(12位：2位TA机构号+1位客户类型+8位序号+1位效验位)
 **返回值：无
 **修改的全局变量：
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

    //初始化
    memset(stmp, 0, sizeof(stmp));
    memset(sTAID, 0, sizeof(sTAID));
    memset(scust, 0, sizeof(scust));

    //区分对私、对公客户获取当前最大序号
    if (pcust_type[0] == PERSONE)
        PubStrncpy(sCur, ppriv_cur, 12);
    else
        PubStrncpy(sCur, ppub_cur, 12);
    //获取客户类型序号
    PubStrncpy(scust, sCur+2, 1);
    icust = atoi(scust);
    //获取8位序号并增加
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
    //区分机器产生奇偶序号
    imode = itmp%2;
    if (imode == 0 && imachine_no == 1
        || imode == 1 && imachine_no == 0)
        itmp++;
    snprintf(sTAID, 12+1, "%-2.2s%1d%08d0", pta_org, icust, itmp);
    //生成客户TA编号效验位
    PubChkVerify(sTAID, 1);
    //返回
    if (pcust_type[0] == PERSONE)
        PubStrncpy(ppriv_cur, sTAID, 12);
    else
        PubStrncpy(ppub_cur, sTAID, 12);
    PubStrncpy(pcust_ta_id, sTAID, 12);
    return;
}

/****
 **编程人员：滕一勤
 **功能：生成流水序号
 **输入参数：机器编号、当前内存流水号
 **输出参数：流水序号(17位：1位机器号+6位时分秒+3位毫秒+7位序号)
 **输出参数：流水序号(19位：1位机器号+6位年月日+6位时分秒+3位毫秒+3位序号)
 **返回值：无
 **修改的全局变量：
****/
static void createFlowNo(int imachine_no, char *pcur, char *pflow_no)
{
    int     itmp = 0;
    int     imode = 0;
    char    stmp[3+1];
    char    sflow_no[19+1];
    struct  timeb sttimeb;
	struct  tm *local;

    //初始化
    memset(stmp, 0, sizeof(stmp));
    memset(sflow_no, 0, sizeof(sflow_no));
    memset(&sttimeb, 0, sizeof(sttimeb));

    //获取当前时间
    ftime(&sttimeb);
	local = localtime(&sttimeb.time);

    //生成流水号
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
    //返回
    PubStrncpy(pflow_no, sflow_no, 19);
    return;
}

/****
 **编程人员：滕一勤
 **功能：根据文件名获取共享内存KEY
 **输入参数：文件名
 **输出参数：无
 **返回值：共享内存KEY
 **修改的全局变量：
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
            , "获取%s指定的KEY出错", filename);
        return -1;
    }
    //返回
    return keySHM;
}

/****
 **编程人员：滕一勤
 **功能：根据文件名获取信号灯KEY
 **输入参数：文件名
 **输出参数：无
 **返回值：信号灯KEY
 **修改的全局变量：
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
            , "获取%s指定的SEM KEY出错", filename);
        return -1;
    }
    //返回
    return keySEM;
}

/****
 **编程人员：滕一勤
 **功能：创建系统信号灯
 **输入参数：信号灯KEY
 **输出参数：信号灯ID
 **返回值：TRUE/FALSE
 **修改的全局变量：
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
                , "创建信号灯失败");
            return FALSE;
        }
        //初始化信号灯
        iInitial = 0;
        if (semctl(semID, FESP_SEMP_NUM, SETALL, iInitial) < 0)
        {
            PubErr( -1, __FILE__, __LINE__, PXYM
                , "初始化信号灯失败");
            return FALSE;
        }
	}
    //返回
    *psem_id = semID;
    return TRUE;
}

/****
 **编程人员：滕一勤
 **功能：获取信号灯锁
 **输入参数：信号灯数组位置、信号灯KEY
 **输出参数：无
 **返回值：TRUE/FALSE
 **修改的全局变量：
****/
static int getSemLock(int isemnum, int isemID)
{
	int     i = 0;
    int     ret = FALSE;

	static struct sembuf stSem_Lock[2]=
	{
		0,0,IPC_NOWAIT|SEM_UNDO,    //等待信号量值为0
		0,1,IPC_NOWAIT|SEM_UNDO	    //对信号量加锁
	};

    if (isemnum < 0 || isemnum > FESP_SEMP_NUM)
    {
        PubErr( -1, __FILE__, __LINE__, PXYM
            , "要获取的信号灯位置%d超过范围0-%d", isemnum, FESP_SEMP_NUM);
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
            , "获取%d信号灯失败。errno=%d", isemID, errno);
	return (ret);
}


/****
 **编程人员：滕一勤
 **功能：解除信号灯锁
 **输入参数：信号灯数组位置、信号灯KEY
 **输出参数：无
 **返回值：TRUE/FALSE
 **修改的全局变量：
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
            , "要释放的信号灯位置%d超过范围0-%d", isemnum, FESP_SEMP_NUM);
        return FALSE;
    }

    stSem_UnLock[0].sem_num = isemnum - 1;

    if ((semop(isemID, &stSem_UnLock[0], 1)) < 0)
        return FALSE;
    else
        return TRUE;
}



