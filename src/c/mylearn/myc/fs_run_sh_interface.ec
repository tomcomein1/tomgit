/*********************************************************
*  ������:   fs_run_sh_interface.ec
*  ��������: ϵͳ����ͳһ����shell����ӿ�ʵ��
**  �ڲ�����:
**
**  �����Ա:  ̷�ľ�
**  ����޸�����: 20090512
*********************************************************/

EXEC SQL include  "fs_mframe.h";

/*��������SHELL�ӿڳ���*/
int RunShellFunction(int taskno)
{
    int configFile[256];
    int line[256];
    int ret = 0;
    FILE *fp = NULL;
    
    PubStrncpy(configFile, "%s/etc/ShellConfig.ini", (char *) getenv("HOME"));
    
    if ( NULL == (fp = fopen(configFile, "r")))
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "���ļ���[%s]", configFile);
        return FALSE;
    }
    
    while (1)
    {
        memset (line, 0, sizeof(line));
        if (PubGetLine(line, fp)<=0) break;
        
        /*����ע�ͻ����*/
        if (line[0] == '#' || line[0] == '\n') 
            continue;
        
        /*���������е�shell����*/
        ret = system(line);
        if (ret != 0)
            PubErr(0, __FILE__, __LINE__, PXYM, "����[%s]����", line);

    }
    
    return TRUE;
}
