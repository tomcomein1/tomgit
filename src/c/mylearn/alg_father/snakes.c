  // <Copyright liaoqb>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
 
struct snake
{
    int x;
    int y;
    char m;
    struct snake *next;
};
 
typedef struct snake Snake;
 
struct queue
{
    Snake *head;
    Snake *tail;
}snakeQueue;
 
int check=0;
struct queue *p=&snakeQueue;
 
void snakeMove( int i,int j );
void outPut( void );
void putMoney( void );
void gameover( void );
void creatSnakes( struct queue *snakes );
void initSnakes( struct queue *snakes );
void enQueue( Snake *p,int value );
void outQueue( Snake *p );
  
char map[ 12 ][ 12 ]={"************",        
                      "*XXXXH     *",       
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "*          *",
                      "************"}; 
 
int main()
{
    char direction;
 
    printf( "Enter 'a' to left,'d' to right,'w' to up,'s' to down\n\n" );
 
    putMoney();
    outPut;
 
    while ( 1 )
    {
        direction=getch();
 
        creatSnakes( p );
 
        while ( !kbhit() )
        {
            switch ( direction )
            {
            case 'w':
                snakeMove( 0,1 );
                break;
 
            case 's':
                snakeMove( 0,-1 );
                break;
 
            case 'a':
                snakeMove( -1,0 );
                break;
 
            case 'd':
                snakeMove( 1,0 );
                break;
            }
                system( "cls" );
 
                outPut();
 
                if ( check==1 )
                    break;
 
                sleep( 500 );
        }
    }
 
    return 0;
}
 
void outPut( void )
{
    int i,j;
 
    for ( i=0;i<12;++i )
    {
        for ( j=0;j<12;++j )
            printf( "%c",map[ i ][ j ] );
 
        printf( "\n" );
    }
}
 
void putMoney( void )
{
    int i,j;
 
    srand( time( NULL ) );
 
    do
    {
        i=rand()%10+1;
        j=rand()%10+1;
    }
    while ( map[ i ][ j ]!=' ' );
 
    map[ i ][ j ]='o';
}
     
void gameover( void )
{
    check=1;
 
    printf( "Game Over!!!< ^ ^ >\n" );
 
    return;
}
 
void initSnakes( struct queue *snakes )
{
    snakes->head=snakes->tail=( Snake * )malloc( sizeof( Snake ) );
 
    snakes->head->next=NULL;
}
 
void creatSnakes( struct queue *snakes )
{
    int i;
 
    initSnakes( snakes );
 
    for ( i=0;i<=5;++i )
    {
        enQueue( snakes,i );
    }
}

