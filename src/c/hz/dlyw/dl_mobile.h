#ifndef DL_MOB_H
#define DL_MOB_H
void MobileSignIn(long nType,char * sItem,char * sJym);
void MobileSignOut(long nType,char * sItem,char * sJym);
void MobileCheckAccount(long ntype,char * sitem,char * sJym);
void MobileCompletePrint(long nType,char * sItem,char * sJym);
long IsValidPhoneNo(char * sPhone);
#endif


