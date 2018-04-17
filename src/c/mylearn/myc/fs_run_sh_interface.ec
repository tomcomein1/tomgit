/*********************************************************
*  程序名:   fs_run_sh_interface.ec
*  功能描述: 系统任务统一调用shell程序接口实现
**  内部函数:
**
**  编程人员:  谭文均
**  最后修改日期: 20090512
*********************************************************/

EXEC SQL include  "fs_mframe.h";

/*共公调用SHELL接口程序*/
int RunShellFunction(int taskno)
{
    int configFile[256];
    int line[256];
    int ret = 0;
    FILE *fp = NULL;
    
    PubStrncpy(configFile, "%s/etc/ShellConfig.ini", (char *) getenv("HOME"));
    
    if ( NULL == (fp = fopen(configFile, "r")))
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "打开文件错[%s]", configFile);
        return FALSE;
    }
    
    while (1)
    {
        memset (line, 0, sizeof(line));
        if (PubGetLine(line, fp)<=0) break;
        
        /*过滤注释或空行*/
        if (line[0] == '#' || line[0] == '\n') 
            continue;
        
        /*运行配置中的shell程序*/
        ret = system(line);
        if (ret != 0)
            PubErr(0, __FILE__, __LINE__, PXYM, "运行[%s]出错！", line);

    }
    
    return TRUE;
}
