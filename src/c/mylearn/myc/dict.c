#include <stdio.h>

int main()
{
	product_dict(6);
	return 0;
}

int product_dict(int passwordmax)
{

    char a[]="0123456789abcdefghijklmnopqrstuvwxyz";
    int ndictcount=sizeof(a); 
    char cpass[passwordmax+1]; 
    int nminl=1,nmaxl=passwordmax; 
    char array[passwordmax]; 
    
    int nlength=nminl;
    register int j,i=0;
    enum bool {false, true} bnext;

    while(nlength<=nmaxl)
    {
        /*for(i=0;i<passwordmax;i++)
            array[i]=0; */
        memset(array, 0, passwordmax);
        bnext=true;
        while(bnext)
        {
            for(i=0;i<nlength;i++)
                cpass[i]=a[ array[i] ];
            cpass[i]='\0';
            printf( "%s\n", cpass );

            for(j=nlength-1;j>=0;j--) /*����ָ���λ */
            { 
                array[j]++; /*��password���λ�Ӷ�Ӧ���������Ϊ�ֵ��ϵ���һλ�ַ���*/
                if(array[j]!=ndictcount-1)
                {
                    /*��������ֵ��ϵ�����û�����꣬�����������١�*/
                    break;
                }
                else 
                {
                    /*���ֽ������Ѹ���������0.������һ�֡�*/
                    array[j]=0;
                    if(j==0)
                      bnext=false;
                }
            }
            
        }
        nlength++;
    } 

    return 0;
}
