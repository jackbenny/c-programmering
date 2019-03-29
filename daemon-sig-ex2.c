#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>

char pidfile[] = "/tmp/daemon-sig-ex.pid";

void sigHandler(int sig);
void shutDown(int sig);

int main(void)
{
    pid_t process_id = 0;
    pid_t sid = 0;
    FILE *fp;

    /* Skapa barn/child process */
    process_id = fork();

    /* Hantera eventuellt fel vid fork() */
    if (process_id < 0)
    {
        fprintf(stderr, "fork misslyckades!\n");
        exit(1);
    }

    /* Förälderprocessen/parent process.
       Avsluta den eftersom allt arbete kommer att
       utföras av demonen. Skriv också ut demonens PID */
    else if (process_id > 0)
    {
        printf("Demonens PID: %d \n", process_id);
        fp = fopen(pidfile, "w");
        fprintf(fp, "%d", process_id);
        fclose(fp);
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
        fprintf(stderr, "Misslyckades att skapa en ny session\n");
        exit(1);
    }
    
    /* Ändra aktuell katalog till / */
    chdir("/");

    /* Stäng stdin, stdout och stderr. Dessa behövs inte
       i en demonprocess */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Registrera signalhanterarna, en för varje signal
       som vi vill fånga upp */
    signal(SIGUSR1, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGABRT, shutDown);
    signal(SIGQUIT, shutDown);
    signal(SIGINT, shutDown);
   
    /* Demonens arbete... */
    for(;;)
    {
        sleep(5);
    }
    return 0;
}

void sigHandler(int sig)
{
    if (sig == SIGUSR1)
    {
        /* Skriv till syslog-demonen */
        syslog(LOG_DAEMON, "Tog emot SIGUSR1");
    }
    else if (sig == SIGTERM)
    {
        /* Samma som ovan */
        syslog(LOG_DAEMON, "Tog emot SIGTERM");
    }
    else
    {
        syslog(LOG_DAEMON, "Någonting hände med demonen...");
    }
    /* Signalhanteraren återställs efter första signalen,
       vi måste därför sätta den igen */
    signal(sig, sigHandler);
}

void shutDown(int sig)
{
    syslog(LOG_DAEMON, "Demonens avslutas");
    /* Ta bort PID-filen efter oss */
    if ( unlink(pidfile) == -1 )
        syslog(LOG_DAEMON, "Kunde inte ta bort PID-filen");
    exit(0);
}
