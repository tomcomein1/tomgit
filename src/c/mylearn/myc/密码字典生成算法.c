char a[]="0123456789abcdefghijklmnopqrstuvwxyz"; 
    int ndictcount=sizeof(a); 
    char cpass[passwordmax+1]; 
    int nminl=1,nmaxl=passwordmax; 
    char array[passwordmax]; 
    
    int nlength=nminl;
    register int j,i=0;
    bool bnext;
    while(nlength<=nmaxl)
    {
        //for(i=0;i<passwordmax;i++)
        //    array[i]=0;
        memset(array, 0, passwordmax);
        bnext=true;
        while(bnext)
        {
            for(i=0;i<nlength;i++)
                cpass[i]=a[array[i] ];
            cpass[i]='\0';
            cout << cpass << endl;
            for(j=nlength-1;j>=0;j--) //密码指针进位
            { 
                array[j]++;//把password最低位加对应的字体更新为字典上的下一位字符。
                if(array[j]!=ndictcount-1)
                {
                    //如果本轮字典上的字体没有数完，则继续本轮穷举。
                    break;
                }
                else 
                {
                    //本轮结束，把各个变量清0.进入下一轮。
                    array[j]=0;
                    if(j==0)
                      bnext=false;
                }
            }
            
        }
        nlength++;
    } 
