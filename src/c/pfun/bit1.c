#include <stdio.h>

int main(void)
{
    struct Bit {
        int b8: 1;
        int b7: 1;
        int b6: 1;
        int b5: 1;
        int b4: 1;
        int b3: 1;
        int b2: 1;
        int b1: 1;
    } B;
    
    unsigned char *p = NULL;

    B.b1 = 1; B.b2 = 1; B.b3 = 1; B.b4 = 1; B.b5 = 1; B.b6 = 1; B.b7 = 1; B.b8 = 1;
    p = (unsigned char *)&B;
    printf("%d\n", *p);  /* 255 */

    B.b1 = 0; B.b2 = 0; B.b3 = 0; B.b4 = 0; B.b5 = 0; B.b6 = 0; B.b7 = 0; B.b8 = 1;
    p = (unsigned char *)&B;
    printf("%d\n", *p);  /* 1   */

    B.b1 = 0; B.b2 = 0; B.b3 = 0; B.b4 = 0; B.b5 = 1; B.b6 = 1; B.b7 = 1; B.b8 = 1;
    p = (unsigned char *)&B;
    printf("%d\n", *p);  /* 15  */

    B.b1 = 0; B.b2 = 1; B.b3 = 1; B.b4 = 1; B.b5 = 1; B.b6 = 1; B.b7 = 1; B.b8 = 1;
    p = (unsigned char *)&B;
    printf("%d\n", *p);  /* 127 */

    getchar();
    return 0;
}
