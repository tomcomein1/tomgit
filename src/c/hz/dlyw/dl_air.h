#ifndef DL_AIR_H
#define DL_AIR_H
static void PreAirTicket();
static void EndAirTicket();
static long InitAirTicket();
static void ShowAirTicketFace();
static long InputAirInfo(char * sJym);
static long QueryAirLine(char * sJym);
static long SelectAirLine(char *sJym);
static long SelectAirLineOne(char *sJym);
static void OnLineHelp(char * sJym,long nSelect);
static long QueryTicketPrice(char * sJym,long nSelect);
static void QueryAirRoute(char * sJym,long nSelect);
static void QueryAirBerth(char * sJym,long nSelect);
static long PurchaseTicket(char * sJym,long nAirLine,long nBerth,long nFlag,char * sLine);
void AirPrintReceipt(char * sLsh);
static long CheckQueryState(char * sJym);
long fillterberth(char source[],char sout[]);
static long SaveAirTicketData(DATATABLE * pdt,char *sJym);
static long SaveAirTiketPay(DATATABLE * pdt,char * outbuf);
static long SaveAirTiketDeliver(DATATABLE * pdt,char * outbuf);
 
     #ifndef TEST
     #define TEST
     #endif
#endif
