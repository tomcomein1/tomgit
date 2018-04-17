/****************************************************************************
 * Licensed Material - Property Of IBM
 *
 * "Restricted Materials of IBM"
 *
 * IBM Informix Client SDK
 *
 * (c)  Copyright IBM Corporation 1997, 2008. All rights reserved.
 *
 ****************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
EXEC SQL include sqlca;
EXEC SQL include sqlda;
EXEC SQL include sqltypes;
EXEC SQL include decimal;
#define FLDSIZE 255
/*datetime type is bug*/

/* This program handles ONLY the following data types:
 *
 *	SMALL FLOAT	FLOAT		MONEY
 *	DECIMAL		CHAR  VARCHAR DATE INTEGER
 *
 * It does not handle any other SQL data type, nor does it
 * handle nulls.  (Adding such support is easy.)
 */

mint has_trans;		/*  True if DB has transactions.  */

int main(int argc, char *argv[])
{

     /* Use the status code returned from the DATABASE
      * statement to check if the database has transactions.
      */

     EXEC SQL database ipssdb;
     has_trans = ( sqlca.sqlwarn.sqlwarn1 == 'W' );
     return unload(argv[1], argv[2]);

}

int unload (slctstmt, fname)

EXEC SQL BEGIN DECLARE SECTION;
    char *slctstmt;
EXEC SQL END DECLARE SECTION;

char *fname;
{
     register mint pos;
     register char *cp;
     register mint len;
     register mint i;
     char field[FLDSIZE+1];
     char *fieldp;
     struct sqlda *udesc;
     struct sqlvar_struct *col;
     FILE *unlfile;
     char delim = '|';
     char *buffer = NULL;

     EXEC SQL prepare usqlobj from :slctstmt;
     if ( sqlca.sqlcode != 0 ) 
     {
	  fprintf(stderr, "sql error! [%d]\n", sqlca.sqlcode);
	  return -1;
     }
     EXEC SQL describe usqlobj into udesc;
     if ( sqlca.sqlcode != 0 ) 
     {
	  fprintf(stderr, "sql error! [%d]\n", sqlca.sqlcode);
	  return -1;
     }
     
     unlfile = fopen(fname,"w");
     if ( unlfile == NULL )
	  {
	  fprintf(stderr,"Cannot open file '%s'\n", fname);
	  return -1;
	  }

     /* Step 1: analyze udesc to determine type and memory requirements
      *         of each item in the select list.  rtypalign() returns a
      *         pointer to the next appropriate boundary (starting at
      *         pos) for the indicated data type.
      */
     pos = 0;
     for (col = udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	  {
	  switch(col->sqltype)
	       {
	       case SQLSMFLOAT:
		    col->sqltype = CFLOATTYPE;
		    break;

	       case SQLFLOAT:
		    col->sqltype = CDOUBLETYPE;
		    break;

	       case SQLMONEY:
	       case SQLDECIMAL:
		    col->sqltype = CDECIMALTYPE;
		    break;

	       case SQLCHAR:
		    col->sqltype = CCHARTYPE;
		    break;
           
           case SQLINT:
           case SQLSERIAL:
            col->sqltype = CINTTYPE;
            break;
           
           case SQLDATE:
            col->sqltype = CDATETYPE;
            break;
           
           case SQLDTIME:
            col->sqltype = CDTIMETYPE;
            break;
           
           case SQLVCHAR:
            col->sqltype = CVCHARTYPE;
            break;

	       default:
		    /*  The program does not handle INTEGER,
		     *  SMALL INTEGER, DATE, SERIAL or other
		     *  data types.  Do nothing if we see
		     *  an unsupported type. 
		     */
		     fprintf (stderr, "Don't support this type: %d", col->sqltype);
		    return -1;
		}
	col->sqllen = rtypmsize(col->sqltype,col->sqllen);
	pos = rtypalign( pos, col->sqltype) + col->sqllen;
	col->sqlind = NULL;
	}

     /* Step 2: Allocate memory to receive a row of the table returned
      *         by the SELECT statement.  The variable pos has an integer
      *         value equal to the number of bytes occupied by the row.
      */
     buffer = malloc(pos);
     if ( buffer == NULL )
	   {
	   fprintf(stderr,"Out of memory\n");
	   return -1;
	   }

     /* Step 3: Set pointers in the allocated memory to receive each
      *         item in the select list.
      */
     cp = buffer;
     for (col = udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	  {
	  cp = (char *) rtypalign( (mlong) cp, col->sqltype);
	  col->sqldata = cp;
	  cp += col->sqllen;
	  }

     /* Step 4: Fetch each row of the query, convert to ASCII format, and
      *         write to the output file.
      */
     EXEC SQL declare usqlcurs cursor for usqlobj;

    if ( has_trans )
         EXEC SQL begin work;

    EXEC SQL open usqlcurs;
    EXEC SQL fetch usqlcurs using descriptor udesc;
    while (sqlca.sqlcode == 0 )	
	{
        for (col=udesc->sqlvar, i = 0; i < udesc->sqld; col++, i++)
	    {
	       byfill(field,FLDSIZE+1,0);
	       fieldp = field;
	       switch (col->sqltype)
		   {

		    case CFLOATTYPE:
			 sprintf(field,"%f",
				 (double) *((float *) (col->sqldata)));
			 break;
		    
		    case CDOUBLETYPE:
			 sprintf(field,"%f",*((double *) (col->sqldata)));
			 break;
		    
		    case CDECIMALTYPE:
			 dectoasc( (dec_t *) col->sqldata, field, FLDSIZE,-1);
			 break;
			 
		    case CCHARTYPE:
			 fieldp = col->sqldata;
			 break;
		    
		    case CINTTYPE:
		     sprintf(field, "%d", *((int *) (col->sqldata)));
             break;
            
            case CDATETYPE:
             rfmtdate(*((int *)(col->sqldata)), "mm/dd/yyyy", fieldp);
             break;
             
            case CDTIMETYPE:
             fieldp =  col->sqldata;
             break;
            
            case CVCHARTYPE:
             fieldp = col->sqldata;
             break;

		    default:
			 /*  Usupported data type.  */
			 break;

		   }
	       len = stleng(fieldp);
	       cp = fieldp + len - 1;
	       while ( len > 1 && *cp == ' ' )
		   {
		    *cp = '\0';
		    len--, cp--;
		   }
	       fputs(fieldp,unlfile);
	       putc(delim,unlfile);
	    }
	putc('\n',unlfile);
        EXEC SQL fetch usqlcurs using descriptor udesc;
	}

     fclose(unlfile);
     free(buffer);
     if ( sqlca.sqlcode && sqlca.sqlcode != SQLNOTFOUND ) 
	 {
	  /* display ERROR message	*/
	  fprintf(stderr,"unload failed sqlcode = %d\n", sqlca.sqlcode);
	 }
     else 
	 {
	  fprintf(stderr,"unloaded %d rows\n" , sqlca.sqlerrd[2]);
	 }

     EXEC SQL close usqlcurs;
     if ( has_trans )
        EXEC SQL commit work;
	free(udesc);
	
	return 0;
}
