#include "zydes.h"
#include "zydesx.h"

int _Encrypt(WORD EncryptKey[8],WORD Source[8],WORD Target[8],int EncryptFlag)
{
    int pKey[64];
    int sKey[16][48];
    WORD tPin[8];
    register int I;
    
    for(I = 0; I < 8; I++)
        tPin[I] = Source[I];
    
    bit2ten(EncryptKey,pKey);
    keyinit(pKey,sKey);
    
    if(EncryptFlag == 1)
        endes(tPin, 1, sKey);
    else
        endes(tPin, 0, sKey);

    for(I = 0; I < 8; I++)
        Target[I] = tPin[I];

    return 0;
}

int DesEncrypt(const char Key[8],const char* ClearStr,char* EncryptedStr)
{
    WORD pKey[8];
    WORD tClear[8];
    WORD tEncrypt[8];
    char tcClear[8];
    char tcEncrypt[17];
    
    int lClearStr = strlen(ClearStr);
    int I = 0,J = 0;
    int iTemp = lClearStr;
    
    for (I = 0; I < 8; I++)
        pKey[I] = Key[I];

    I = 0;
   /*
    pKey[0] = 0x35;  pKey[4] = 0x35; 
    pKey[1] = 0x31;  pKey[5] = 0x31; 
    pKey[2] = 0x35;  pKey[6] = 0x35; 
    pKey[3] = 0x31;  pKey[7] = 0x31; 
   */ 
    
    
    strcpy(EncryptedStr,"");
    while (I < lClearStr)
    {
        memset(tcClear,0,sizeof(tcClear));
        memset(tcEncrypt,0,sizeof(tcEncrypt));
        iTemp = iTemp - 8;
        if (iTemp >= 0)
        {
            memcpy(tcClear,ClearStr+I,8);
            memset(&tClear, 0x00, sizeof(int) * 8); /*不足8位的密码要用0x00补足8位*/
            for(J = 0; J < 8 && J < strlen(tcClear); J ++)
                tClear[J] = tcClear[J];
            _Encrypt(pKey,tClear,tEncrypt,1);
            for(J = 0; J < 8; J ++)
            sprintf(tcEncrypt + J * 2, "%02X", tEncrypt[J]); /*转成字符串*/
            strcat(EncryptedStr,tcEncrypt);
        }
        else {
            iTemp = iTemp + 8;
            memcpy(tcClear,ClearStr+J,iTemp);
            memset(&tClear, 0x00, sizeof(int) * 8); /*不足8位的密码要用0x00补足8位*/
            for(J = 0; J < 8 && J < strlen(tcClear); J ++)
                tClear[J] = tcClear[J];
            _Encrypt(pKey,tClear,tEncrypt,1);
            for(J = 0; J < 8; J ++)
            sprintf(tcEncrypt + J * 2, "%02X", tEncrypt[J]); /*转成字符串*/
            strcat(EncryptedStr,tcEncrypt);
        }
        I = I+8;
    }
    return 0;
}

int DesDecrypt(const char Key[8],const char* EncryptedStr,char* ClearStr)
{
    WORD pKey[8];
    WORD tClear[8];
    WORD tEncrypt[8];
    char tcClear[9];
    char tcEncrypt[17];
    
    int lEncryptedStr = strlen(EncryptedStr);
    int I = 0,J = 0;
    
    for (I = 0; I < 8; I++) {
        pKey[I] = Key[I];
        tEncrypt[I] = EncryptedStr[I];
    }

    I = 0;
    /*
    pKey[0] = 0x35;  pKey[4] = 0x35; 
    pKey[1] = 0x31;  pKey[5] = 0x31; 
    pKey[2] = 0x35;  pKey[6] = 0x35; 
    pKey[3] = 0x31;  pKey[7] = 0x31; 
    */
    
    strcpy(ClearStr,"");

/*    
    if ((lEncryptedStr % 16) != 0) return 1;
    while (I < lEncryptedStr)
    {
        memset(tcClear,0,sizeof(tcClear));
        memset(tcEncrypt,0,sizeof(tcEncrypt));
        memcpy(tcEncrypt,EncryptedStr + I,16);
        for (J = 0;J < 8;J++)
        {
            tEncrypt[J] = ASC2BCD(tcEncrypt + (J * 2));
            tEncrypt[J] = tcEncrypt[J];
        }
*/

        _Encrypt(pKey,tEncrypt,tClear,0);
        for (J = 0;J < 8;J++)
            ClearStr[J] = tClear[J];

/*
        strcat(ClearStr,tcClear);
        I = I+8
    }
*/
    return 0;        
}


/*
main()
{
    char uid[16];
    char puid[256];
    char uuid[16];
    //strcpy(uid,"123456789012345");
    strcpy(uid,"123456");
    DesEncrypt(uid,puid);
    printf("uid = [%s]\n",uid);
    printf("puid = [%s]\n",puid);
    
    DesDecrypt(puid,uuid);
    printf("uid = [%s]\n",uuid);
    printf("puid = [%s]\n",puid);
}
*/

