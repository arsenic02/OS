/*
Korišćenjem programskog jezika C napisati Linux/UNIX program koji kreira dodatni process sa kojim komunicira dvosmerno korišćenjem jednog reda poruka. Proces roditelj iz datoteke čije je ime prosleđeno
kao argument komandne linije čita liniju po liniju teksta i svaku liniju šalje procesu detetu u posebnoj poruci. Proces dete u svakoj primljenoj liniji prebrojava broj pojavljivanja velikih slova 
abecede i procesu roditelju vraća kroz isti red poruka za svaku analiziranu liniju teksta kroz posebnu poruku redni broj linije i broj pojavljivanja velikih slova. Glavni program te podatke štampa na 
ekranu (redni broj linije i broj pojavljivanja velikih slova). Kada se analiziraju sve linije teksta iz datoteke glavni program na ekranu treba da odštampa “KRAJ”. (Obavezno da se zatvore jer ako se 
napune neće hteti dalje da šalju i primaće neki stari sadržaj. Ako ne javlja ni jednu grešku kompajler i nije segfault i program šlogirava promeniti broj reda i ima šanse da pomogne. Kreirati novu 
promenljivu pre svakog slanja, izgleda izvodi egzibicije sa memorijom i desi se problem kad se koristi jedna za svako slanje.)
*/
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
struct mymsgbuf
{
    long mtype;
    char mtext[20];
};
int main(int argc, char *argv[])
{
    int msqid = msgget(10161, 0666 | IPC_CREAT);
    if (fork() != 0)
    {
        int i = 0;
        struct mymsgbuf buf;
        while (1)
        {
            msgrcv(msqid, &buf, sizeof(buf), 1, 0);
            if (!strcmp(buf.mtext, "KRAJ"))
                break;
            int j = 0;
            int k = 0;
            while (buf.mtext[j] != 0)
            {
                if (buf.mtext[j] >= 'A' && buf.mtext[j] <= 'Z')
                    k++;
                j++;
            }
            struct mymsgbuf buf1;
            buf1.mtype = 2;
            char bb[2];
            sprintf(bb, "%d", i);
            strcpy(buf1.mtext, bb);
            strcat(buf1.mtext, "-");
            sprintf(bb, "%d", k);
            strcat(buf1.mtext, bb);
            msgsnd(msqid, &buf1, sizeof(buf1), 0);
            i++;
        }

        struct mymsgbuf buf1;
        buf1.mtype = 2;
        strcpy(buf1.mtext, "KRAJ");
        msgsnd(msqid, &buf1, sizeof(buf1), 0);

        buf.mtype = 1;
        msgrcv(msqid, &buf, sizeof(buf), 1, 0);
    }
    else
    {
        FILE *f = fopen(argv[1], "r");
        while (!feof(f))
        {
            struct mymsgbuf buf;
            buf.mtype = 1;
            fgets(buf.mtext, 20, f);
            msgsnd(msqid, &buf, sizeof(buf), 0);
        }
        fclose(f);
        struct mymsgbuf buf;
        buf.mtype = 1;
        strcpy(buf.mtext, "KRAJ");
        msgsnd(msqid, &buf, sizeof(buf), 0);

        struct mymsgbuf buf1;
        buf1.mtype = 2;
        while (1)
        {
            msgrcv(msqid, &buf1, sizeof(buf1), 2, 0);
            printf("%s\n", buf1.mtext);
            if (!strcmp(buf1.mtext, "KRAJ"))
                break;
        }

        msgsnd(msqid, &buf, sizeof(buf), 0);
    }
    msgctl(msqid, IPC_RMID, NULL);
}
