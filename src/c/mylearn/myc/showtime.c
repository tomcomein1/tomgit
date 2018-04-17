#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int showtime()
{
  struct timeval tv;
  struct  tm  *ptm;/*¡Ÿ ±±‰¡ø*/

  char stime[100];

  gettimeofday( &tv, (struct timezone *)NULL);
  ptm = localtime( &( tv.tv_sec ));

  memset (stime, 0x00, sizeof(stime));
  strftime (stime, sizeof(stime), "%T:", ptm);
  snprintf( stime + strlen(stime),
          sizeof(stime) - strlen(stime),
          "%06ld", tv.tv_usec );

  fprintf (stdout, "%s\n", stime);
  fflush(stdout);

  return 0;


}

void main()
{
        showtime();
}
