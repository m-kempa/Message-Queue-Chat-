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
#include <time.h>



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
    int tab_zablokowanych[9];
    int tab_zablokowanych_grup[3];
};

struct grupa {
  char nazwa_g[100];
  int tab_nalezacych[9];
};


struct uzytkownik lista_uzytkownikow[9];
struct grupa lista_grup[3];

int tab_kolejek[9];


int main()
{
    
    FILE* plik = fopen("plik.txt", "r");
    
    int ind = 234567;
    char bufor[100];
    
    for(int i =0; i<9; i++)
    {
        fgets(bufor, 100, plik);
        bufor[strlen(bufor)-1] = '\0';
        strcpy(lista_uzytkownikow[i].nazwa_u, bufor);
        
        
        fgets(bufor, 100, plik);
        bufor[strlen(bufor)-1] = '\0';
        strcpy(lista_uzytkownikow[i].haslo, bufor);
        lista_uzytkownikow[i].ID = ind;
        lista_uzytkownikow[i].zalogowany = 0;
        lista_uzytkownikow[i].p_log = 4;
        ind+=10;
        for(int j =0; j<9; j++){
         lista_uzytkownikow[i].tab_zablokowanych[j] = 0;
        }
        for(int k =0; k<9; k++){
         lista_uzytkownikow[i].tab_zablokowanych_grup[k] = 0;
        }
        
    }
    
    int indeks;
    
    for(int i=0; i<3; i++){
        
        fgets(bufor,100,plik);
        bufor[strlen(bufor)-1] = '\0';
        strcpy(lista_grup[i].nazwa_g, bufor);
        for(int j =0; j<9; j++){
         lista_grup[i].tab_nalezacych[j] = -1;
        }
        
        fgets(bufor, 100, plik);
        bufor[strlen(bufor) - 1] = '\0';
        indeks = atoi(bufor);
        lista_grup[i].tab_nalezacych[indeks] = lista_uzytkownikow[indeks].ID;
        
        fgets(bufor, 100, plik);
        bufor[strlen(bufor) - 1] = '\0';
        indeks = atoi(bufor);
        lista_grup[i].tab_nalezacych[indeks] = lista_uzytkownikow[indeks].ID;
    
    }
    printf("\nLista uzytkownikow \n\n");
    for (int i=0; i<9; i++)
    {
        printf("Nazwa uzytkownika: %s \n", lista_uzytkownikow[i].nazwa_u);
        printf("Haslo uzytkownika: %s \n", lista_uzytkownikow[i].haslo);
        printf("ID uzytkownika: %d \n", lista_uzytkownikow[i].ID);
        printf("Zalogowany: %d \n", lista_uzytkownikow[i].zalogowany);
        printf("Proby logowania: %d \n\n\n", lista_uzytkownikow[i].p_log);
    }
    
    fclose(plik);
    
    time_t curtime;
    time(&curtime);
    
    for(int i = 0; i<9; i++){
        tab_kolejek[i]= msgget(lista_uzytkownikow[i].ID, IPC_CREAT | 0644);
    }
    
    int id;
    int odbior;
    
    struct wiad_kom wiad;
    
    id = msgget(12345678, IPC_CREAT | 0644);
    
    while(1){
        for(int u = 0; u<9; u++){
            
            //---------LOGOWANIE (typ odebranego komunikatu: 1)-------
            odbior=0;
            odbior = msgrcv(id, &wiad, (sizeof(wiad) - sizeof(long)),1, IPC_NOWAIT);
            if(odbior>0){
              struct wiad_kom wiad_zwr;
              wiad_zwr.typ = 20;
              int istnieje = 0;
              for(int i =0; i<9; i++){
                strcpy(wiad_zwr.wiadomosc, "");
                if(strcmp(wiad.nazwa_u, lista_uzytkownikow[i].nazwa_u) == 0 && lista_uzytkownikow[i].p_log == 0){
                    istnieje = 1;
                    strcpy(wiad_zwr.wiadomosc, "");
                    strcpy(wiad_zwr.wiadomosc, "jest zablokowany \n");
                    msgsnd(id, &wiad_zwr, (sizeof(wiad_zwr)-sizeof(long)),0);
                }
                else{
                if(strcmp(wiad.nazwa_u, lista_uzytkownikow[i].nazwa_u) == 0 && strcmp(wiad.haslo, lista_uzytkownikow[i].haslo) != 0) {
                    strcpy(wiad_zwr.wiadomosc, "");
                    istnieje = 1;
                    lista_uzytkownikow[i].p_log = lista_uzytkownikow[i].p_log -1;
                    int proby = lista_uzytkownikow[i].p_log;
                    char nr[2];
                    sprintf(nr, "%d",proby);
                    strcat(wiad_zwr.wiadomosc, ": niepoprawne haslo. Pozostałe próby logowania: ");
                    strcat(wiad_zwr.wiadomosc, nr );
                    msgsnd(id, &wiad_zwr, (sizeof(wiad_zwr)-sizeof(long)),0);
                    printf("Niepoprawne haslo serwer\n");
                    }
                    
                if(strcmp(wiad.nazwa_u, lista_uzytkownikow[i].nazwa_u) == 0 && strcmp(wiad.haslo, lista_uzytkownikow[i].haslo) == 0) {
                    istnieje = 1;
                    if(lista_uzytkownikow[i].zalogowany==0){
                        strcpy(wiad_zwr.wiadomosc, "");
                        lista_uzytkownikow[i].zalogowany = 1;
                        wiad_zwr.id = lista_uzytkownikow[i].ID;
                        printf("Zalogowano dobrze po str serwera\n");
                        strcpy(wiad_zwr.wiadomosc, "poprawnie zalogowany!\n");
                        msgsnd(id, &wiad_zwr, (sizeof(wiad_zwr)-sizeof(long)),0);
                        printf("Numer kom %d\n",tab_kolejek[0]);
                    }
                    else{
                        strcpy(wiad_zwr.wiadomosc, "");
                        strcpy(wiad_zwr.wiadomosc, "jest juz zalogowany! \n");
                        msgsnd(id, &wiad_zwr, (sizeof(wiad_zwr)-sizeof(long)),0);
                    }
                }
              }
              }
              if(istnieje==0){
                  strcpy(wiad_zwr.wiadomosc, " o podanych danych nie istnieje!!\n");
                  msgsnd(id, &wiad_zwr, (sizeof(wiad_zwr)-sizeof(long)),0);
              }
              strcpy(wiad_zwr.wiadomosc, "");
            }
            
            
            //WYLOGOWANIE UZYTKOWNIKA (typ odebranego komunikatu: 13)
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)),13, IPC_NOWAIT);
            if(odbior>0){
             lista_uzytkownikow[u].zalogowany = 0;
             printf("Wylogowano po stronie serwera \n");
            }
            
            
            //LISTA GRUP (typ odebranego komunikatu: 3)
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 3, IPC_NOWAIT);
            if(odbior>0){
              struct wiad_kom wiad_lista_grup;
              wiad_lista_grup.typ = 20;
              strcpy(wiad_lista_grup.wiadomosc, "");
              for( int i=0; i<3; i++){
                   char nr[2];
                   int numer = i+1;
                   sprintf(nr, "%d",numer);
                   strcat(wiad_lista_grup.wiadomosc, "Gr nr.");
                   strcat(wiad_lista_grup.wiadomosc, nr );
                   strcat(wiad_lista_grup.wiadomosc, "\t");
                   strcat(wiad_lista_grup.wiadomosc, lista_grup[i].nazwa_g);
                   strcat(wiad_lista_grup.wiadomosc, "\n");
               
              }
              msgsnd(tab_kolejek[u],&wiad_lista_grup, (sizeof(wiad_lista_grup)-sizeof(long)),0);
              printf("Po stronie serwera lista grup poprawnie\n");
            }
            
            
            //LISTA ZALOGOWANYCH UZYTKOWNIKOW (typ odebranego komunikatu: 4)
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 4, IPC_NOWAIT);
            if(odbior>0){
              struct wiad_kom wiad_lista_uz;
              wiad_lista_uz.typ = 20;
              strcpy(wiad_lista_uz.wiadomosc, "");
              for( int i=0; i<9; i++){
               if(lista_uzytkownikow[i].zalogowany == 1){
                   strcat(wiad_lista_uz.wiadomosc, lista_uzytkownikow[i].nazwa_u);
                   strcat(wiad_lista_uz.wiadomosc, "\n");
               }
              }
              msgsnd(tab_kolejek[u],&wiad_lista_uz, (sizeof(wiad_lista_uz)-sizeof(long)),0);
              printf("Po str serwera lista zalogowanych uzytkownikow poprawnie \n");
            }
            
            //LISTA UZYTKOWNIKOW W GRUPACH TEMATYCZNYCH (typ odebranego komunikatu: 5)
           odbior =0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 5, IPC_NOWAIT);
            if(odbior > 0)
            {
                int istnieje = 0;
                struct wiad_kom wiad_ugr;
                wiad_ugr.typ = 20;
                strcpy(wiad_ugr.wiadomosc, ""); // "zerowanie" bufora
                for(int i=0; i<3; i++){
                   if(strcmp(lista_grup[i].nazwa_g, wiad.nazwa_g) == 0) {
                    istnieje = 1;
                    strcat(wiad_ugr.wiadomosc, lista_grup[i].nazwa_g);
                    strcat(wiad_ugr.wiadomosc, " :\n");
                    for(int j=0; j<9; j++){
                        if(lista_grup[i].tab_nalezacych[j] != -1){
                         
                            strcat(wiad_ugr.wiadomosc, lista_uzytkownikow[j].nazwa_u);
                            strcat(wiad_ugr.wiadomosc, "\n");
                        }
                    }
                   }
                }
                if(istnieje == 0){
                    strcpy(wiad_ugr.wiadomosc, "Podana Grupa nie istnieje");
                }
                msgsnd(tab_kolejek[u],&wiad_ugr, (sizeof(wiad_ugr)-sizeof(long)),0);
                printf("Po str serwera lista uz nal do gr poprawnie \n");
            } 
            
            //------Dolaczenie/odejscie  grupy tematycznej (typ odebranego komunikatu: 6)----
            odbior =0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 6, IPC_NOWAIT);
			if (odbior > 0) {
				struct wiad_kom wiad_dol;
				wiad_dol.typ = 20;
                strcpy(wiad_dol.wiadomosc, "");
				for (int i = 0; i < 3; i++)
				{
					if (strcmp(wiad.nazwa_g, lista_grup[i].nazwa_g) == 0) {
						if (lista_grup[i].tab_nalezacych[u] == lista_uzytkownikow[u].ID) {
							lista_grup[i].tab_nalezacych[u] = -1;
							strcpy(wiad_dol.wiadomosc, "Usunieto z grupy tematycznej\n");
							msgsnd(tab_kolejek[u], &wiad_dol, (sizeof(wiad_dol) - sizeof(long)), 0);
						}
						else {
							lista_grup[i].tab_nalezacych[u] = lista_uzytkownikow[u].ID;
							strcpy(wiad_dol.wiadomosc, "Dodano do grupy tematycznej\n");
							msgsnd(tab_kolejek[u], &wiad_dol, (sizeof(wiad_dol) - sizeof(long)), 0);
						}
					}
				}

			}
            
            //----Wiadomosc do uzytkownika (typ odebranego komunikatu: 8)-----
            
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 8, IPC_NOWAIT);
            if(odbior>0){
                struct wiad_kom wiad_wyslana;
                struct wiad_kom wiad_zwrotna;
                strcpy(wiad_wyslana.wiadomosc, "");
                strcpy(wiad_zwrotna.wiadomosc, "");
                int istnieje = 0;
                char od[100]; 
                strcpy(od, lista_uzytkownikow[u].nazwa_u);
                wiad_zwrotna.typ = 20;
                for( int i=0; i<9; i++){
                 if(strcmp(wiad.nazwa_u, lista_uzytkownikow[i].nazwa_u) == 0){
                        istnieje=1;
                        wiad_wyslana.typ = 21;
                        strcpy(wiad_wyslana.nazwa_u, od);
                        strcpy(wiad_wyslana.wiadomosc, wiad.wiadomosc);
                        strcpy(wiad_wyslana.godzina, ctime(&curtime));
                        msgsnd(tab_kolejek[i], &wiad_wyslana, (sizeof(wiad_wyslana) - sizeof(long)),0);
                        //printf("%d\n",i);
                        strcpy(wiad_zwrotna.wiadomosc, "Wysylanie wiadomosci do uzytkownika zakonczone suksesem");
                        msgsnd(tab_kolejek[u], &wiad_zwrotna,(sizeof(wiad_zwrotna)-sizeof(long)),0);
                        break;
                        
                 }
                }
                if(istnieje==0){
                 strcpy(wiad_zwrotna.wiadomosc, "Wystapil blad, wysylanie nie powiodlo sie");
                 msgsnd(tab_kolejek[u], &wiad_zwrotna,(sizeof(wiad_zwrotna)-sizeof(long)),0);
                }
            }
            
            //-----Wiadomosc do grupy (typ odebranego komunikatu: 9)----
            
            
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 9, IPC_NOWAIT);
            if(odbior>0){
                struct wiad_kom wiad_wyslana_gr;
                struct wiad_kom wiad_zwrotna;
                strcpy(wiad_wyslana_gr.wiadomosc, "");
                strcpy(wiad_zwrotna.wiadomosc, "");
                int istnieje = 0;
                char od[100]; 
                strcpy(od, lista_uzytkownikow[u].nazwa_u);
                wiad_zwrotna.typ = 20;
                for(int i=0; i<3;i++){
                 if(strcmp(wiad.nazwa_g,lista_grup[i].nazwa_g) == 0){
                     istnieje =1;
                     for( int j = 0; j<9; j++){
                        if(lista_grup[i].tab_nalezacych[j] != -1){
                         wiad_wyslana_gr.typ = 21;
                         strcpy(wiad_wyslana_gr.nazwa_u, od);
                         strcpy(wiad_wyslana_gr.wiadomosc, wiad.wiadomosc);
                         strcpy(wiad_wyslana_gr.godzina, ctime(&curtime));
                         msgsnd(tab_kolejek[j], &wiad_wyslana_gr, (sizeof(wiad_wyslana_gr) - sizeof(long)), 0);
                         //printf("%d\n",j);
                         strcpy(wiad_zwrotna.wiadomosc, "Wiadomosc do grupy zakonczone sukcesem");
                         msgsnd(tab_kolejek[u],&wiad_zwrotna, (sizeof(wiad_zwrotna) - sizeof(long)),0);
                        }
                    }
                 }
                }
                if(istnieje == 0){
                 strcpy(wiad_zwrotna.wiadomosc, "Blad przy wysylaniu wiadomosci do grupy");
                 msgsnd(tab_kolejek[u],&wiad_zwrotna, (sizeof(wiad_zwrotna) - sizeof(long)),0);
                }
            }
            
            //------Blokowanie uzytkonika (typ odebranego komunikatu: 10)------
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 10, IPC_NOWAIT);
            if(odbior>0){
                struct wiad_kom wiad_blokuj_u;
                strcpy(wiad_blokuj_u.wiadomosc, "");
                wiad_blokuj_u.typ = 20;
                int istnieje = 0;
                for(int i=0; i<9; i++){
                   if(strcmp(lista_uzytkownikow[i].nazwa_u, wiad.nazwa_u) == 0) {
                    istnieje = 1;
                    lista_uzytkownikow[u].tab_zablokowanych[i] = 1;
                         
                    strcat(wiad_blokuj_u.wiadomosc, lista_uzytkownikow[i].nazwa_u);
                    strcat(wiad_blokuj_u.wiadomosc, " zostal zablokowany\n");
                
                    }
                   }
                if(istnieje == 0){
                    strcpy(wiad_blokuj_u.wiadomosc, "Podany uzytkownik nie istnieje");
                }
                msgsnd(tab_kolejek[u],&wiad_blokuj_u, (sizeof(wiad_blokuj_u)-sizeof(long)),0);
                printf("Po str serwera blokowanie ok\n");
            }
            
            //------Blokowanie grupy (typ odebranego komunikatu: 11)------
            odbior = 0;
            odbior = msgrcv(tab_kolejek[u], &wiad, (sizeof(wiad) - sizeof(long)), 11, IPC_NOWAIT);
            if(odbior>0){
                struct wiad_kom wiad_blokuj_g;
                strcpy(wiad_blokuj_g.wiadomosc, "");
                wiad_blokuj_g.typ = 20;
                int istnieje = 0;
                for(int i=0; i<3; i++){
                   if(strcmp(lista_grup[i].nazwa_g, wiad.nazwa_g) == 0) {
                    istnieje = 1;
                    lista_uzytkownikow[u].tab_zablokowanych_grup[i] = 1;
                         
                    strcat(wiad_blokuj_g.wiadomosc, lista_grup[i].nazwa_g);
                    strcat(wiad_blokuj_g.wiadomosc, " zostala zablokowany\n");
                
                    }
                   }
                if(istnieje == 0){
                    strcpy(wiad_blokuj_g.wiadomosc, "Podana grupa nie istnieje");
                }
                msgsnd(tab_kolejek[u],&wiad_blokuj_g, (sizeof(wiad_blokuj_g)-sizeof(long)),0);
                printf("Po str serwera blokowanie grup ok\n");
            }
            
            //----Przeslanie zbioru zablokowanych uzytkownikow i grup( typ odebranego komunikatu: 12)
            
            
    }
    }
    return 0;
}


