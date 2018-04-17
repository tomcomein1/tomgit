#include <stdio.h>
#include <fcntl.h>

main()
{
        int stream;
        char buf[] = "hello world!";

        stream = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644 );
        printf ("stream: %d\n", stream);
        if (stream < 0)
        {
                perror ("open test.txt ");
                exit (0);
        }

        write (stream, buf, sizeof(buf) );

        close (stream);

}
