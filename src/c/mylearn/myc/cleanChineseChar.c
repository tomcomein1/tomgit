
/*��ȫ���ַ���ɿո�*/
int cleanChineseChar(char *pszString)
{
    char *p;
    p = pszString;
    for(;*p != '\0'; p++)
    {
        if((*p)&0x80)
            *p = ' ';
    }
    return TRUE;
}
