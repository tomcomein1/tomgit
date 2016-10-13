#include <stdio.h>
#include <stdlib.h>

int day_diff(int year_start, int month_start, int day_start
   , int year_end, int month_end, int day_end)
{
 int y2, m2, d2;
 int y1, m1, d1;
 
 m1 = (month_start + 9) % 12;
 y1 = year_start - m1/10;
 d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);

 m2 = (month_end + 9) % 12;
 y2 = year_end - m2/10;
 d2 = 365*y2 + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + (day_end - 1);
 
 return (d2 - d1);
}

int datediff(long bdate, long edate)
{
        int byear, bmonth, bday;
        int eyear, emonth, eday;

        byear = bdate / 10000;
        bmonth = bdate/100 - byear*100;
        bday = bdate % 100;

        eyear = edate / 10000;
        emonth = edate/100 - eyear*100;
        eday = edate % 100;
        return day_diff(byear, bmonth, bday, eyear, emonth, eday);
}

int cdatediff(char *bdate, char *edate)
{
        return datediff(atol(bdate), atol(edate) );
}

int main(void)
{
 printf("%d\n", day_diff(2015, 1, 1, 2015, 1, 8));
 printf("%d\n", day_diff(2015, 1, 29, 2015, 2, 9));
 printf("%d\n", datediff(20161023, 20161123) );
 printf("%d\n", cdatediff("20161023", "20161120") );
 
 return 0;
}