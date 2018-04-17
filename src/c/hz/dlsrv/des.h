/*************************************
**
**函数: DesEncString
**目的: 加密给定长度的串(可为字符串，二进制串，各种类型数组等)
**参数: sKey 密钥串. 一般为8个字节长(实际加密时只用其前8个字节)
**      nKeyLen 密钥长度. 如果密钥为字符串(以'\0'结尾)，值可为0; 
**              如果密钥不足8个字节, 密钥串后面不足的字节以'\0'填充
**      sSrc 加密前的串. 一般为8个倍数字节长
**      nSrcLen 加密前的串长度. 如果该串为字符串(以'\0'结尾)，值可为0; 
**              如果该串长度不足8的倍数个字节, 该串后面不足的字节以'\0'填充
**      sDest 加密后的串. 为8个倍数字节长
**      pnDestLen 加密后的串长度指针. 须先赋存放加密后的串的缓冲长度值,
**                函数执行后返回加密后的串长度
**                如果该指针为NULL, 表示缓冲区足够大并且不需要返回加密后的串长度 
**返回值: 0 成功
**        -1 密钥长度或加密前的串长度或加密后的串长度小于0
**        -2 存放加密后的串的缓冲不够大
**        -3 密钥为空或串指针为空,或者加密前的串为空或串指针为空,或者加密后的串指针为空
**
*************************************/
int DesEncString(char* sKey, int nKeyLen, char* sSrc, int nSrcLen, char* sDest, int* pnDestLen);

/*************************************
**
**函数: DesDecString
**目的: 解密给定长度的串
**参数: sKey 密钥串. 一般为8个字节长(实际解密时只用其前8个字节)
**      nKeyLen 密钥长度. 如果密钥为字符串(以'\0'结尾)，值可为0; 
**              如果密钥不足8个字节, 密钥串后面不足的字节以'\0'填充
**      sSrc 解密前的串. 为8个倍数字节长
**      nSrcLen 解密前的串长度. 该串长度必须为8的倍数个字节
**      sDest 存放解密后的串. 为nSrcLen字节长, 如果是字符串, 调用者自己负责在尾部加'\0'
**      nDestLen 存放解密后的串的缓冲长度值. 如果该长度为0, 表示缓冲区足够大(至少nSrclen)
**返回值: 0 成功
**        -1 密钥长度或解密前的串长度或解密后的串长度小于0
**        -2 存放解密后的串的缓冲不够大
**        -3 密钥为空或串指针为空,或者加密前的串为空或串指针为空,或者加密后的串指针为空
**        -4 加密前的串长度为0或不是8的倍数
**
*************************************/
int DesDecString(char* sKey, int nKeyLen, char* sSrc, int nSrcLen, char* sDest, int nDestLen);


