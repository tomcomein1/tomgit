
#ifndef T_JYMGL_H
#define T_JYMGL_H
#define MODE_CUSTOMER    0x00000001
#define MODE_IDENTIFIED  0x00000002
long dsTaskCreateDsJym(char * sJym,char * sJymc,char * sSjydz,long nCxms);
long dsTaskCreateSpJym(char * sJym,char * sJymc,long nMode);
long dsTaskOpenJym(char * sJym,char * sJsdh,char * sWhy,long nMode);
long dsTaskDeleteJym(char * sJym);
long dsTaskCreateDbJym(char * sJym,char * sJymc,char * sSjydz,long nCxms);
long dsTaskCheckOpen(char * sJym,char * sJh);

#endif
