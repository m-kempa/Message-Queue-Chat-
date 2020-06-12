#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


struct wiad_kom{
    long typ;
    char nazwa_u[100];
    char haslo[100];
    char nazwa_g[100];
    char wiadomosc[2000];
    char godzina[100];
    int id;
    
};

struct uzytkownik {
	char nazwa_u[100];
	char haslo[100];
    int ID;
    int zalogowany;
    int p_log;
};

struct uzytkownik uzytk; //uztkownik korzystajacy z klienta

int ident;
int wyjscie;


int main()
{
    uzytk.zalogowany = 0;
    
    while(wyjscie != 1){
        int wybor;
        printf("-------CHAT------\n");
        printf("1.Zaloguj uzytkonika\n");
        printf("2.Wyloguj uzytkonika\n");
        printf("3.Lista dostepnych grup tematycznych\n");
        printf("4.Lista zalogowanych uzytkonikow\n");
        printf("5.Lista uzytkownikow w grupach tematycznych\n");
        printf("6.Dolacz do grupy tematycznej\n");
        printf("7.Wypisz sie z grupy tematycznej\n");
        printf("8.Wyslij wiadomosc do uzytkownika\n");
        printf("9.Wyslij wiadomosc do grupy\n");
        printf("10.Zablokuj uzytkownika\n");
        printf("11.Zablokuj grupe tematyczna\n");
        printf("12.Odbierz wiadomosc\n");
        printf("13.Wyjdz z aplikacji\n");
        scanf("%d", &wybor);
        
        switch(wybor){
            case 1:
            {
                //-----------LOGOWANIE UZYTKOWNIKA----------
                int id = msgget(12345678, IPC_CREAT | 0644); 
                struct wiad_kom wiad;
                struct wiad_kom wiad_zwr;
                printf("Podaj login: \n");
                scanf("%s",uzytk.nazwa_u);
                printf("Podaj haslo: \n");
                scanf("%s",uzytk.haslo);
                strcpy(wiad.nazwa_u, uzytk.nazwa_u);
                strcpy(wiad.haslo, uzytk.haslo);
                wiad.typ = 1;
                msgsnd(id, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(id, &wiad_zwr, (sizeof(wiad_zwr) - sizeof(long)), 20, 0);
                uzytk.ID = wiad_zwr.id;
                uzytk.zalogowany = 1;
                printf("Uzytkownik %s %s\n", uzytk.nazwa_u, wiad_zwr.wiadomosc);
                ident = msgget(uzytk.ID, IPC_CREAT | 0644);
                
             
             
             break;   
            }
            case 2:
            {
             //---------WYLOGOWANIE UZYTKOWNIKA-----
            
                int spr = 0;
                printf("Na pewno chcesz wylogować uzytkownika %s (1- TAK / 0- NIE)?\n", uzytk.nazwa_u);
                scanf("%d", &spr);
                struct wiad_kom wiad;
                if(spr == 1){
                    wiad.typ = 13;
                    wiad.id = uzytk.ID;
                    uzytk.zalogowany = 0;
                    msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                    printf("wylogowano \n");
                }
                break;
            }
            case 3:
            {
                //-------Lista grup tematycznych
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.id = uzytk.ID;
                wiad.typ = 3;
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("Lista grup tematycznych: \n");
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
                
            }
            case 4:
            {
                //---------Lista zalogowanych uzytkownikow-----
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.id = uzytk.ID;
                wiad.typ = 4;
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("Lista zalogowanych uzytkownikow: \n");
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
                
            }
            case 5:
            {
             //----------Lista uzytkownikow nalezacych do grupy tematycznej
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.id = uzytk.ID;
                wiad.typ = 5;
                printf("Podaj nazwe grupy, ktorej uzytkownikow chcesz wypisac: \n");
                scanf("%s", wiad.nazwa_g);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("Lista czlonkow grupy tematycznej ");
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
            }
            case 6:
            {
            //------Dolaczanie do grupy tematycznej
                
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.typ = 6;
                wiad.id = uzytk.ID;
                printf("Podaj nazwe grupy tematycznej do ktorej chcesz dolaczyc : \n");
                scanf("%s", wiad.nazwa_g);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
            }
            case 7:
            {
                //------Wypisz sie z grupy tematycznej
                
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.typ = 6;
                wiad.id = uzytk.ID;
                printf("Podaj nazwe grupy tematycznej z ktorej chcesz sie wypisac : \n");
                scanf("%s", wiad.nazwa_g);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
            }
            case 8:
            {
             //----Wysylanie wiadomosci do uzytkownika
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.typ = 8;
                wiad.id = uzytk.ID;
                strcpy(wiad.nazwa_u , uzytk.nazwa_u);
                printf("Wpisz odbiorce wiadomosci: \n");
                scanf("%s", wiad.nazwa_u);
                printf("Wpisz tresc wiadomosci: \n");
                scanf(" %[^\t\n]s", &wiad.wiadomosc);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                break;
            }
            case 9:
            {
                ////-------Wysylanie wiadomosc do grupy
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.typ = 9;
                wiad.id = uzytk.ID;
                strcpy(wiad.nazwa_u , uzytk.nazwa_u);
                printf("Wpisz nazwe grupy: \n");
                scanf("%s", wiad.nazwa_g);
                printf("Wpisz tresc wiadomosci: \n");
                scanf(" %[^\t\n]s", &wiad.wiadomosc);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                break;
                
            }
            case 10:
            {   
                //------Zablokuj uzytkownika
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.id = uzytk.ID;
                wiad.typ = 10;
                printf("Podaj nazwe uzytkownika ktorego chcesz zablokowac: \n");
                scanf("%s", wiad.nazwa_u);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;   
            }
            case 11:
            {
                //------Zablokuj grupe
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                wiad.id = uzytk.ID;
                wiad.typ = 11;
                printf("Podaj nazwe grupy ktora chcesz zablokowac: \n");
                scanf("%s", wiad.nazwa_g);
                msgsnd(ident, &wiad, (sizeof(wiad) - sizeof(long)), 0);
                msgrcv(ident, &wiad, (sizeof(wiad) - sizeof(long)), 20, 0);
                printf("%s \n", wiad.wiadomosc);
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break; 
                
            }
            case 12:
            {
                ///------Odbieranie wiadomosci
                struct wiad_kom wiad;
                strcpy(wiad.wiadomosc, "");
                while(msgrcv(ident, &wiad, (sizeof(wiad)- sizeof(long)), 21, IPC_NOWAIT) > 0){
                    printf("\nOd %s: \t %s\n", wiad.nazwa_u, wiad.godzina);
                    printf("%s \n\n", wiad.wiadomosc);  
                }
                printf("Nacisnij klawisz aby kontynuowac.\n");
                getchar();
                break;
            }
            case 13:
            {
                /////------Wyjscie z aplikacji
                int spr;
                printf("Chcesz wyjsc z aplikacji? (1-Tak/0-Nie)\n");
                scanf("%d",spr);
                if(spr == 1){
                 wyjscie = 1;   
                }
                break;
            }
            default:
            {
                printf("Nie ma takiej opcji!");
            }
        }
        
    }
    
 
    return 0;
}
