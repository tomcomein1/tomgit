#define PRINTD(x) printf("%s : %d\n", #x, (x));

main()
{
        int a = 10;
        #ifdef AIX_UNIX
        __typeof(a) b = 20 ;
        #endif

        PRINTD(a);
        #ifdef AIX_UNIX
        PRINTD(b);
        #endif

        getch();
}
