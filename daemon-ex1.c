#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(void)
{
    pid_t process_id = 0;
    pid_t sid = 0;

    /* Skapa barn/child process */
    process_id = fork();

    /* Hantera eventuellt fel vid fork() */
    if (process_id < 0)
    {
        fprintf(stderr, "fork failed!\n");
        exit(1);
    }

    /* Förälderprocessen/parent process.
       Avsluta den eftersom allt arbete kommer att
       utföras av demonen. Skriv också ut demonens PID */
    else if (process_id > 0)
    {
        printf("Daemon process: %d \n", process_id);
        exit(0);
    }

    /* Sätt umask till 000 så att alla kan komma eventuella
       filer skapade av demonen. Anpassas efter vad demonen
       ska användas till */
    umask(0);
    /*  Starta en ny session */
    sid = setsid(); 

    if(sid < 0)
    {
        /* Avsluta ifall det inte gick att skapa en ny
           session */
        fprintf(stderr, "Failure to set new session\n");
        exit(1);
    }
    
    /* Ändra aktuell katalog till / */
    chdir("/");

    /* Stäng stdin, stdout och stderr. Dessa behövs inte
       i en demonprocess */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Här börjar demonens arbete... */
    for(;;)
    {
        sleep(5);
    }
}
