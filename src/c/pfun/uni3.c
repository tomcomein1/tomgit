#include <stdio.h>

int main(void)
{
    enum UnionTyte{SHORT, INT, FLOAT};
    
    struct Rec {
        unsigned ID;
        enum UnionTyte type; /* 识别字段 */
        union {              /* 联合 */
            short s;
            int   i;
            float f;
        } u;
    } R[4];

    size_t i;
    
    R[0].ID = 1;
    R[0].type = SHORT;
    R[0].u.s = 123;

    R[1].ID = 2;
    R[1].type = INT;
    R[1].u.i = 12345;

    R[2].ID = 3;
    R[2].type = FLOAT;
    R[2].u.f = 3.14;

    R[3].ID = 4;
    R[3].type = INT;
    R[3].u.i = 54321;
    
    for (i = 0; i < sizeof R / sizeof R[0]; i++) {
        switch (R[i].type) {
            case SHORT : printf("%u: %hd\n", R[i].ID, R[i].u.s); break;
            case INT   : printf("%u: %d\n", R[i].ID, R[i].u.i);  break;
            case FLOAT : printf("%u: %g\n", R[i].ID, R[i].u.f);  break;
        }   
    }
    
    getchar();
    return 0;
}

