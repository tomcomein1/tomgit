#include <stdlib.h>
#include <stdio.h>


//recorder

enum state { s_stop, s_play, s_forward, s_backward, s_pause, s_record  };
enum message { play, stop, forward, backward, record, pause };


void state_change(enum message m)
{
    static enum state s = s_stop;
    switch (s)
    {
    case s_play:
        if(m == stop)
        {
            s = s_stop;
            printf("stop.\n");
        }
        else if (m == pause)
        {
            s = s_pause;
            printf("pause");
        }
        break;
    case s_pause:
        if(m == pause)
        {
            s = s_play;
            printf("play.\n");
        }
        else if(m == stop)
        {
            s = s_stop;
            printf("stop.\n");
        }
        break;
    case s_stop:
        if(m == play)
        {
            s = s_play;
            printf("play.\n");
        }
        if(m == backward)
        {
            s = s_backward;
            printf("backward.\n");
        }
        if(m == forward)
        {
            s = s_forward;
            printf("forward.\n");
        }
        if(m == record)
        {
            s = s_record;
            printf("record.\n");
        }
        break;
    case s_forward:
        if(m == stop)
        {
            s = s_stop;
            printf("stop.\n");
        }
        break;
    case s_backward:
        if(m == stop)
        {
            s = s_stop;
            printf("stop.\n");
        }
        break;
    case s_record:
        if(m == stop)
        {
            s = s_stop;
            printf("stop.\n");
        }
        break;


    }

}


int main(int argc, char *argv[])
{
    char c = 0x00;
    while(1)
    {
        c = getchar();
        switch(c)
        {
        case ' ':
            state_change(pause);
            break;
        case 'p':
            state_change(play);
            break;
        case 'r':
            state_change(record);
            break;
        case 's':
            state_change(stop);
            break;
        case 'f':
            state_change(forward);
            break;
        case 'b':
            state_change(backward);
            break;
        case 'q':
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
