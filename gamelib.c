#include "gamelib.h" 

static Cell_t *chessboard = NULL;
static Pg_t Ninja;
static Pg_t Ciccio;
static Pg_t *turn=NULL;
static int ncell=0;

static void CreateMap(bool *mcheck); //function for the map creation and printing menu
static void CreateChessboard(void); //function to create the Chessboard
static void PrintChessboard(int n, bool checkplay); //function to print Chessboard
static void useobject(void); //function to use objects collected during the game
static int gamemenu(bool *end, short *conta); //in-game menu
static void strtolower(char *str); //homemade function to emulate other languages "strtolower" function
static void printfile(char *str, char *color); //to print an ASCII art txt file on the screen
static int Inserisci(char *string); //function to ease the insertion of probabilities and to manage exceptions
static void Warning(void); //When called, this functions print an error message
static void Printstatus(void); //to print player's state
static void NameAssignment(Pg_t *player, char *str); //to assign player's name in the right field
static void movepg(int m); //function to move players and trigger events
static void trapped(void); //triggered by a trap
static void aliencombat(void); //triggered by an alien
static void resrand(void); //function to shrink chessboard
static void finalround(bool *randturn); //triggered by the presence of both players in the same cell
static void addtower(void); //to add an element in the tails of tower's linked list
static void deletetower(Pg_t *enemy, bool *l); //to remove an element in the tails of tower's linked list
static void Salva(void); //to save actual chessboard in a bin file
static void Carica(void); //to load a chessboard from a bin file
static void cleanlists(Tw *temp); //to clean player's linked lists
static void cleanbuffer(void); //to clean stdin's buffer

void Menu(bool *check){
    char scelta[50];
    bool mapcheck=0;
    do{
        printfile("Forsennite.txt",COL_RED);

        printf("Benvenuto, cosa vuoi fare? [gioca, imposta mappa, salva, carica, esci]: %s", COL_BLUE);
        fgets(scelta, 50, stdin);
        printf("%s", COL_WHITE);
        strtolower(scelta);
    }while(strncmp(scelta, "imposta", 7)!=0 && strncmp(scelta, "esci", 4)!=0 && strncmp(scelta, "gioca", 5)!=0 && strncmp(scelta, "salva", 5)!=0 && strncmp(scelta, "carica", 6)!=0);

    switch(scelta[0]){
        case 'c':
            Carica();
            break;
        case 's':
            if(chessboard==NULL){
                printf("non c'e\' alcuna scacchiera da salvare!!!!!!");
                
                cleanbuffer();
            }
            else Salva();
            break;
        case 'g':
            if (chessboard == NULL){
                printf("\n%sLa scacchiera non e\' stata ancora creata!!!%s\n",COL_RED,COL_WHITE);
                cleanbuffer();
            }
            else *check=1;
            break;
        case 'i':
            do{
                CreateMap(&mapcheck);
            }while(!mapcheck);
            break;
        case 'e':
            clr;
            cleanchess();
            printfile("credits.txt", COL_YELLOW);
            exit(EXIT_SUCCESS);
            break;
    }
}

static void CreateMap(bool *mcheck){
    char scelta[50];
    do{
        printfile("Creamappa.txt",COL_YELLOW);

        printf("Come vuoi procedere? [crea scacchiera, stampa scacchiera, termina creazione]: %s", COL_BLUE);
        fgets(scelta, 50, stdin);
        printf("%s", COL_WHITE);
        strtolower(scelta);
    }while(strncmp(scelta, "termina", 7)!=0 && strncmp(scelta, "crea", 4)!= 0 && strncmp(scelta, "stampa", 6)!=0 );

    switch(scelta[0]){
        case 'c':
            CreateChessboard();
            break;
        case 's':
            if (chessboard==NULL){
                printf("\n%sLa scacchiera non e\' stata ancora creata!!!%s\n",COL_RED,COL_WHITE);
                sleep(2);
            }
            else{        
                PrintChessboard(ncell, 0);
                cleanbuffer();
            }
            break;
        case 't':
            if (chessboard==NULL){
                printf("\n%sLa scacchiera non e\' stata ancora creata!!!%s\n",COL_RED,COL_WHITE);
                sleep(2);
            }
            else *mcheck=1;
            break;
    }
}

static void CreateChessboard(void){
    printfile("Creascacchiera.txt", COL_GREEN);
    printf("%s", COL_WHITE);
    int al, tra, nodanj, obj, p_med, p_pot, p_mat, p_ammo, x;
    do{
        ncell=Inserisci("inserire il lato della scacchiera di superfice (n*n)");
    }while(ncell==0);
    clr;
    printfile("Creascacchiera.txt", COL_GREEN);
    do{
        al=Inserisci("inserisci la probabilita\' di trovare un alieno in una cella");
        tra=Inserisci("inserisci la probabilita\' di trovare una trappola in una cella");
        nodanj=Inserisci("inserisci la probabilita\' di trovare nessun pericolo in una cella");
    }while((al+tra+nodanj)!=100);
    do{
    obj=Inserisci("inserisci la probabilita\' di trovare un oggetto in una cella");
    }while(obj>100||obj<0);
    do{
        p_med=Inserisci("inserisci la probabilita\' di trovare un medikit in una cella");
        p_pot=Inserisci("inserisci la probabilita\' di trovare una pozione in una cella");
        p_mat=Inserisci("inserisci la probabilita\' di trovare del materiale in una cella");
        p_ammo=Inserisci("inserisci la probabilita\' di trovare delle munizioni in una cella");
    }while((p_med+p_pot+p_mat+p_ammo)!=100);

    chessboard=(Cell_t*) malloc((ncell*ncell)*sizeof(Cell_t));

    for(int i=0; i<ncell; i++){
        for(int j=0; j<ncell; j++){
            x=prob;
            if(x<al) chessboard[i*ncell+j].dancell =alien;
            else{
                if(x<al+tra) chessboard[i*ncell+j].dancell =trap;
                else chessboard[i*ncell+j].dancell =no_danger;
            }
            x=prob;
            if(x<obj){
                x=prob;
                if(x<100-p_ammo-p_mat-p_pot) chessboard[i*ncell+j].objcell = medikit;
                else{
                    if(x<100-p_ammo-p_mat) chessboard[i*ncell+j].objcell = potion;
                    else{
                        if(x<100-p_ammo) chessboard[i*ncell+j].objcell = materials;
                        else chessboard[i*ncell+j].objcell = rocket_launcher_bullet;
                    }
                }

            }
            else{
                chessboard[i*ncell+j].objcell = no_objects;
            }
            chessboard[i*ncell+j].pg=0;
        }
    }
    do{
    x=rand()%ncell;
    Ciccio.x=x;
    x=rand()%ncell;
    Ciccio.y=x;
    chessboard[Ciccio.x*ncell+Ciccio.y].pg=1;
    x=rand()%ncell;
    Ninja.x=x;
    x=rand()%ncell;
    Ninja.y=x;
    chessboard[Ninja.x*ncell+Ninja.y].pg=1;
    }while(Ciccio.x==Ninja.x && Ciccio.y==Ninja.y);
    NameAssignment(&Ninja, "Ninja");
    NameAssignment(&Ciccio, "Ciccio");
    Ciccio.state=life_shield;
    Ninja.state=life_shield;
    for(int i=0; i<4; i++){
            Ciccio.zaino[i]=no_objects;
    }
    for(int i=0; i<4; i++){
            Ninja.zaino[i]=no_objects;
    }
    Ciccio.FTower=NULL;
    Ciccio.LTower=NULL;
    Ninja.FTower=NULL;
    Ninja.LTower=NULL;
    cleanbuffer();
}

static void PrintChessboard(int n, bool checkplay){
    if(checkplay==1){
        printf("<%s*%s> -> Alieni\n<%s*%s> -> Giocatore\n\n",COL_RED,COL_WHITE,COL_BLUE,COL_WHITE);
    }
    else{
        printf("<%s*%s> -> Alieni\n<%s*%s> -> Trappole\n<%s*%s> -> Oggetti\n<%sN %s-%s C%s> -> Giocatori\n\n", COL_RED,COL_WHITE,COL_GREEN,COL_WHITE,COL_YELLOW,COL_WHITE,COL_BLUE,COL_WHITE,COL_BLUE,COL_WHITE);
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(checkplay==1){
                if(i==turn->x && j==turn->y) printf("%s*  ", COL_BLUE);
                else{
                    if(chessboard[i*n+j].dancell==alien){
                    printf("%s*  ", COL_RED);
                    }
                    else printf("%s*  ", COL_WHITE);
                }
                }
            else{
                if(chessboard[i*n+j].pg==1){
                    if(Ciccio.x==i && Ciccio.y==j) printf("%sC  ",COL_BLUE);
                    if(Ninja.x==i && Ninja.y==j) printf("%sN  ",COL_BLUE);
                }
                else{
                    if(chessboard[i*n+j].dancell==trap){
                        printf("%s*  ",COL_GREEN);
                    }
                    else{
                        if(chessboard[i*n+j].dancell==alien){
                            printf("%s*  ", COL_RED);
                        }
                        else{
                            if(chessboard[i*n+j].objcell!=no_objects){
                                printf("%s*  ", COL_YELLOW);
                            }
                            else{
                                printf("%s*  ", COL_WHITE);
                            }
                        }
                    }
                }
            }
        }
        printf("\n");
    }
    printf("%s",COL_WHITE);
}

static void strtolower(char *str){
    int len = strlen(str), i;
        for(i=0;i<len;i++)
            if(str[i]>='A' && str[i]<='Z')
                 str[i] = str[i]-'A'+'a';
}

static void printfile(char *str, char *color){
    char dir[50]="./ASCII/";
    char c;
    strcat(dir, str);
    printf("%s",color);
    FILE *fors=fopen(dir, "r"); 
    clr;
    while((c=getc(fors))!=EOF){
        printf("%c", c);
    }
    fclose(fors);
    printf("%s\n\n", COL_WHITE);
}

void play(void){
    short move,conta=0;
    bool checkfinalstate=0,endturn;
    Ciccio.turn=0;
    Ninja.turn=0;
    bool randturn= rand()%2;
    if(randturn==0) turn=&Ciccio;
    else turn=&Ninja;
    clr;
    printf("Cominci tu la partita! %s", turn->name);
    
    cleanbuffer();
    clr;
    do{
        endturn=0;
        conta++;
        move=gamemenu(&endturn,&conta);
        if(endturn==1){
            movepg(move);
            turn->turn++;
            conta=0;
            if(turn->state!=dead){
                if(turn->turn%(ncell/2)==0 && turn->turn!=0) resrand();
                else{
                    printf("%d turni al restringimento della mappa",(-(ncell/2)+turn->turn));
                    
                    cleanbuffer();
                }
                if (randturn==0){
                    turn=&Ninja;
                    randturn=1;
                }
                else{
                    turn=&Ciccio;
                    randturn=0;
                }
            }
        }
        if(Ciccio.x==Ninja.x && Ciccio.y==Ninja.y) checkfinalstate=1;
    }while(Ninja.state!=dead && Ciccio.state!=dead && !checkfinalstate);
    clr;
    if(checkfinalstate && Ninja.state!= dead && Ciccio.state!=dead) finalround(&randturn);
    if(Ninja.state==dead && Ciccio.state==dead){
        printf("%sEntrambi i giocatori sono morti!%s\n\n",COL_RED,COL_WHITE);
        
        cleanbuffer();
    }
    if(Ciccio.state==dead && Ninja.state!=dead){
        printf("%sil giocatore Ciccio e\' morto! Ninja ha vinto!%s\n\n", COL_BLUE,COL_WHITE);
        
        cleanbuffer();
    }
    if(Ninja.state==dead && Ciccio.state!=dead){
        printf("%sil giocatore Ninja e\' morto! Ciccio ha vinto!%s\n\n", COL_BLUE,COL_WHITE);
        
        cleanbuffer();
    }
}

static int gamemenu(bool *end, short *conta){
    char scelta[50], dir[10];
    if(turn->turn==0 && *conta==1){
        clr;
        printf("%sBenvenuto in FORSENNITE!, nel menu\' di gioco potrai scegliere se:\n-Salvare la partita\n-Muoverti\n-Usare oggetti\n-Uscire\nTieni presente che muovendoti porrai fine al tuo turno!\n\nPremi un tasto per continuare...", COL_GREEN);
        
        cleanbuffer();
        clr;
        printf("Durante il corso della partita vedrai comparire altri menu\', per l'attacco, per la sostituzione di oggetti e per lo scontro finale.\nMuoviti velocemente verso il centro facendo attenzione a trappole e alieni, piu\' alieni sconfiggerai piu\' probabilita\' avrai di sconfiggere il tuo avversario\n\n L'unica cosa fondamentale rimasta da dire e\':\nBUONA FORTUNA COMBATTENTE, L'ARENA DI FORSENNITE TI ASPETTA!\n\nPremi un tasto per continuare...%s", COL_WHITE);
        
        cleanbuffer();
        clr;
    }
    do{
        clr;
        printf("Questo è il tuo turno numero: %s%d%s\nNumero degli alieni uccisi: %s%d%s\nGiocatore: %s%s%s", COL_YELLOW,turn->turn,COL_WHITE,COL_RED,turn->kills,COL_WHITE,COL_BLUE,turn->name,COL_WHITE);
        Printstatus();
        printf("\n\nCome vuoi procedere? [avanza, controlla zaino, usa oggetto, mappa, esci]: %s", COL_BLUE);
        fgets(scelta, 50, stdin);
        printf("%s", COL_WHITE);
        strtolower(scelta);
    }while(strncmp(scelta, "a", 1)!=0 && strncmp(scelta, "c", 1)!=0 && strncmp(scelta, "u", 1)!=0 && strncmp(scelta, "e", 1)!=0 && strncmp(scelta, "m", 1)!=0);

    switch(scelta[0]){
        case 'a':
            do{ 
                clr;
                printf("in che direzione ti vuoi muovere? [up, down, right, left, annulla]: %s", COL_BLUE);
                fgets(dir, 10, stdin);
                strtolower(dir);
                printf("%s", COL_WHITE);
            }while(strncmp(dir, "u", 1)!=0 && strncmp(dir, "d", 1)!=0 && strncmp(dir, "r", 1)!=0 && strncmp(dir, "l", 1)!=0 && strncmp(dir, "a", 1)!=0);

            if(dir[0]!='a')*end=1;

            switch(dir[0]){
                case 'u':
                    return 0;
                case 'd':
                    return 1;
                case 'l':
                    return 2;
                case 'r':
                    return 3;
                case 'a':
                    return 10;
                default:
                    break;
            }
            break;
        case 'c':
            clr;
            printf("Zaino:\n\n");
            for(int i=0; i<4; i++){
                switch(turn->zaino[i]){
                    case 0:
                        printf("%d) Empty\n", i);
                        break;
                    case 1:
                        printf("%d) Medikit\n", i);
                        break;
                    case 2:
                        printf("%d) Potion\n", i);
                        break;
                    case 3:
                        printf("%d) Materials\n", i);
                        break;
                    case 4:
                        printf("%d) Rocket launcher ammo\n", i);
                        break;
                }
            }
            
            cleanbuffer();
            clr;
            break;
        case 'u':
            useobject();
            break;
        case 's':
            break;
        case 'e':
            clr;
            printfile("credits.txt", COL_YELLOW);
            exit(EXIT_SUCCESS);
            break;
        case 'm':
            PrintChessboard(ncell, 1);
            
            cleanbuffer();
            break;
    }
    return 10;
}

static int Inserisci(char *string){ 
  int c=0, x;
  do{
    if(c==1) Warning();
    printf("%s%s: %s ", COL_WHITE, string, COL_BLUE);
    c=0;
    scanf("%d", &x);
    c++;
  }while(x<0);
  printf("%s",COL_WHITE);
  return x;
}

static void Warning(void){ 
  clr;
  printf("\n%sil valore inserito non e\' corretto", COL_RED);
  printf("\nsi prega di inserire un valore %sPOSITIVO!%s\n", COL_YELLOW, COL_GRAY);
  
  cleanbuffer();
  printf("%s",COL_WHITE);
  clr;
}

static void Printstatus(void){
    switch(turn->state){
        case 0:
            printf("\nStato giocatore:%s vulnerabile %s", COL_RED, COL_WHITE);
            break;
        case 1:
            printf("\nStato giocatore:%s vita e scudo %s", COL_BLUE, COL_WHITE);
            break;
        case 2:
            printf("\nStato giocatore:%s solo scudo %s", COL_YELLOW, COL_WHITE);
            break;
        case 3:
            printf("\nStato giocatore:%s solo vita %s", COL_YELLOW, COL_WHITE);
            break;
    }
}

static void NameAssignment(Pg_t *player, char *str){
    for(int i=0;i<strlen(str);i++){
        player->name[i]=*(str+i);
    }
}

static void useobject(void){
    char scelta[50], control;
    do{
        clr;
        printf("quale oggetto vuoi usare?: %s", COL_BLUE);
        fgets(scelta,50,stdin);
        printf("%s", COL_WHITE);
        strtolower(scelta);
        if (strncmp(scelta,"materials",9)==0){
            printf("%snon puoi usare questo oggetto in questo momento!!%s", COL_RED,COL_WHITE);
            
            cleanbuffer();
        }
    }while(strncmp(scelta, "medikit", 7)!=0 && strncmp(scelta, "potion", 6)!=0 && strncmp(scelta, "rocket launcher ammo", 20)!=0 && strncmp(scelta, "annulla", 7)!=0);

    control=scelta[0];
    bool check=0;

    switch(control){
        case 'm':
            for(int i=0; i<4; i++){
                if(turn->zaino[i]==medikit){
                    check=1;
                    turn->zaino[i]=no_objects;
                    break;
                }
            }
            if(!check){
                clr;
                printf("Purtroppo questo oggetto non e\' presente nel tuo zaino.");
                
                cleanbuffer();
            }
            else{
                if(turn->state==vulnerable) turn->state=only_life;
                else{
                    if(turn->state==only_shield) turn->state=life_shield;
                    else 
                        printf("non hai necessita\' di usare questo oggetto!"),
                        
                        cleanbuffer();
                }
            }
            break;
        case 'p':
            for(int i=0; i<4; i++){
                if(turn->zaino[i]==potion){
                    check=1;
                    turn->zaino[i]=no_objects;
                    break;
                }
            }
            if(!check){
                clr;
                printf("Purtroppo questo oggetto non e\' presente nel tuo zaino.");
                
                cleanbuffer();
            }
            else{
                if(turn->state==vulnerable) turn->state=only_shield;
                else{
                    if(turn->state==only_life) turn->state=life_shield;
                    else 
                        printf("non hai necessita\' di usare questo oggetto!"),
                        
                        cleanbuffer();
                }
            }
            break;
        case 'r':
            printf("%snon puoi usare questo oggetto in questo momento!!%s", COL_RED,COL_WHITE);
            
            cleanbuffer();
            break;
    }
}

static void movepg(int m){
    Obj_t control;
    chessboard[turn->x*ncell+turn->y].pg=0;
    switch(m){
        case 0:
            if(turn->x-1<0){
                printf("hai raggiunto il bordo della mappa!, non puoi andare in questa direzione!, sei quasi caduto di sotto");
                
                cleanbuffer();
                clr;
            }
            else turn->x--;
            break;
        case 1:
            if(turn->x+1>ncell-1){
                printf("hai raggiunto il bordo della mappa!, non puoi andare in questa direzione!, sei quasi caduto di sotto");
                
                cleanbuffer();
                clr;
            }
            else turn->x++;
            break;
        case 2:
            if(turn->y-1<0){
                printf("hai raggiunto il bordo della mappa!, non puoi andare in questa direzione!, sei quasi caduto di sotto");
                
                cleanbuffer();
                clr;
            }
            else turn->y--;
            break;
        case 3:
            if(turn->y+1>ncell-1){
                printf("hai raggiunto il bordo della mappa!, non puoi andare in questa direzione!");
                
                cleanbuffer();
            }
            else turn->y++;
            break;
    }
    chessboard[turn->x*ncell+turn->y].pg=1;
    
    if(chessboard[turn->x*ncell+turn->y].dancell==trap){
        trapped();
        if(turn->state==dead) return;
    }
    if(chessboard[turn->x*ncell+turn->y].dancell==alien){
        aliencombat();
        if(turn->state==dead) return;
    }
    if(chessboard[turn->x*ncell+turn->y].objcell!=no_objects){
        if(chessboard[turn->x*ncell+turn->y].objcell==medikit) control=medikit;
        if(chessboard[turn->x*ncell+turn->y].objcell==potion) control=potion;
        if(chessboard[turn->x*ncell+turn->y].objcell==rocket_launcher_bullet) control=rocket_launcher_bullet;
        if(chessboard[turn->x*ncell+turn->y].objcell==materials) control=materials;

        bool check=0;
        short index=0;

        for(int i=0; i<4; i++){
            if(turn->zaino[i]==no_objects){
                check=1;
                index=i;
                break;
            };
        }

        if(check){
            clr;
            printf("%sHai trovato un oggetto!\n\ne\' stato aggiunto allo zaino!%s",COL_GREEN,COL_WHITE);
            turn->zaino[index]=control;
            chessboard[turn->x*ncell+turn->y].objcell=no_objects;
            
            cleanbuffer();
        }
        else{
            int c;
            clr;
            printf("%sHai trovato un oggetto... ma il tuo zaino e\' pieno!\n",COL_GREEN);
            for(int i=0; i<4; i++) printf("\n%d) %d",i+1, turn->zaino[i]);
            printf("\nQuale oggetto vuoi sostituire? [1,2,3,4,5(nessuno)]: %s",COL_BLUE);
            scanf("%d", &c);
            printf("%s",COL_WHITE);
            cleanbuffer();
            switch(c){
                case 1:
                    turn->zaino[0]=control;
                    chessboard[turn->x*ncell+turn->y].objcell=no_objects;
                    break;
                case 2:
                    turn->zaino[1]=control;
                    chessboard[turn->x*ncell+turn->y].objcell=no_objects;
                    break;
                case 3:
                    turn->zaino[2]=control;
                    chessboard[turn->x*ncell+turn->y].objcell=no_objects;
                    break;
                case 4:
                    turn->zaino[3]=control;
                    chessboard[turn->x*ncell+turn->y].objcell=no_objects;
                    break;
                case 5:
                    break;
            }
        }
    }
}

static void trapped(void){
    int x=rand()%2, try;
    do{
    clr;
    printf("%sOps... hai attivato una trappola\nora dovrai affidarti all'RNG per sopravvivere\ninserisci 0 o 1, se non indovinerai la trappola ti uccidera\'!: %s",COL_GRAY,COL_YELLOW);
    scanf("%d",&try);
    cleanbuffer();
    }while(try!=0 && try!=1);
    clr;
    if(x!=try){
        turn->state=dead;
        printf("%sLA TRAPPOLA TI HA UCCISO!%s\n\n", COL_RED,COL_WHITE);
    }
    else printf("%sSei salvo... per ora%s\n\n",COL_GREEN,COL_WHITE);
    
    cleanbuffer();
    clr;

}

static void aliencombat(void){
    int danno=0, x;
    char scelta[50];
    clr;
    printfile("alien2.txt",COL_WHITE);
    
    cleanbuffer();
    do{
        do{
            do{
                clr;
                printf("%sti trovi faccia a faccia con un alieno!\n\ncombatti corpo a corpo oppure provi ad uitilizzare il lanciarazzi?\n[corpo a corpo, lanciarazzi, usa oggetto]: %s",COL_RED,COL_BLUE);
                fgets(scelta,50,stdin);
                strtolower(scelta);
                printf("%s",COL_WHITE);
            }while(strncmp(scelta,"c",1)!=0 && strncmp(scelta,"l",1)!=0 && strncmp(scelta,"u",1)!=0);

            switch(scelta[0]){
                case 'u':
                    useobject();
                    break;
                case 'c':
                    clr;
                    x=prob;
                    if(x>50){
                        danno++;
                        printf("COLPITO!");
                    }
                    else printf("MANCATO!");
                    
                    cleanbuffer();
                    clr;
                    if(danno==2){
                        printf("\nL'alieno e\' stato sconfitto!!!!!!!!");
                        turn->kills++;
                        
                        cleanbuffer();
                        clr;
                    }
                    break;
                case 'l':
                    x=prob; 
                    bool ammo=0;
                    for(int i=0; i<4; i++){
                        if(turn->zaino[i]==rocket_launcher_bullet){
                            ammo=1;
                            turn->zaino[i]=no_objects;
                            break;
                        }
                    }
                    if(ammo){
                        if(x>60){
                            danno=2;
                            printf("Il colpo e\' andato a segno!\n\nL'alieno e\' stato sconfitto!!!!!!!!");
                            turn->kills++;
                            clr;
                        }
                        else printf("purtroppo il colpo ha mancato il bersaglio...."), clr;

                    }
                    else printf("Non hai munizioni!!!!!!!!!");
                    
                    cleanbuffer();
                    break;
            }
        }while(scelta[0]!='c');

        x=prob;
        if(x>60 && danno!=2){
            printf("\n\n\n%sl'alieno ti ha colpito!%s",COL_RED,COL_WHITE);
            switch(turn->state){
                case vulnerable:
                    turn->state=dead;
                    printf("\nSEI MORTO!");
                    break;
                case only_life:
                    turn->state=vulnerable;
                    break;
                case only_shield:
                    turn->state=vulnerable;
                    break;
                case life_shield:
                    turn->state=only_life;
                    break;
            }
            
            cleanbuffer();
        } 
    }while(danno != 2 && turn->state!=dead);
}

static void resrand(void){
    bool aliveC=0, aliveN=0;
    int x=ncell/2;
    PrintChessboard(ncell,0);
    
    cleanbuffer();
    Cell_t *appo= (Cell_t*) calloc(x*x,sizeof(Cell_t));
    for(int i=0; i<x; i++){
        for(int j=0; j<x; j++){
            appo[i*x+j].dancell=chessboard[(i+x/2)*ncell+(j+x/2)].dancell;
            appo[i*x+j].objcell=chessboard[(i+x/2)*ncell+(j+x/2)].objcell;
            appo[i*x+j].pg=chessboard[(i+x/2)*ncell+(j+x/2)].pg;
            if(appo[i*x+j].pg==1){
                if(Ciccio.x==(i+x/2) && Ciccio.y==(j+x/2)){
                    Ciccio.x=i;
                    Ciccio.y=j;
                    aliveC=1;
                }
            }
            if(appo[i*x+j].pg==1){
                if(Ninja.x==(i+x/2) && Ninja.y==(j+x/2)){
                    Ninja.x=i;
                    Ninja.y=j;
                    aliveN=1;
                }
            }
        }
    }

    if(!aliveC) Ciccio.state=dead;
    if(!aliveN) Ninja.state=dead;

    chessboard= (Cell_t*) realloc(chessboard,(x*x)*sizeof(Cell_t)); 
    for(int i=0; i<x; i++){
        for(int j=0; j<x; j++){
            chessboard[i*x+j].dancell=appo[i*x+j].dancell;
            chessboard[i*x+j].objcell=appo[i*x+j].objcell;
            chessboard[i*x+j].pg=appo[i*x+j].pg;
        }
    }
    ncell=x;
    PrintChessboard(ncell,0);
    
    cleanbuffer();
    if(Ninja.state == dead || Ciccio.state == dead) return;
    free(appo);
}

static void finalround(bool *randturn){
    Pg_t *enemy;
    Tw *temp;
    char scelta[50];
    int finalturns=0,x,pshoot=0;
    bool directlauncher =0, bocheck=0, bocheck1=0;

    if (*randturn==0){
        turn=&Ninja;
        *randturn=1;
        enemy=&Ciccio;
    }
    else{
        turn=&Ciccio;
        *randturn=0;
        enemy=&Ninja;
    }

    do{
        if(finalturns<=1){
            printf("hai incontrato il tuo rivale! PREPARATI A COMBATTERE!!!!!!!!!!\n\n");
            printf("%sSparando con la pistola avrai una chance di colpire il tuo nemico, che aumentera\' in base al numero di alieni uccisi in precedenza e diminuira\' in base all'altezza della torre del tuo nemico.\npuoi costruire e alzare una torre a tua volta con i materiali o utilizzare un colpo di lanciarazzi per distruggere una componente della torre.\npuoi anche provare ad utilizzare il lanciarazzi per colpire il tuo avversario e ucciderlo istantaneamente, ma le chance di successo sono molto basse.\n\nIn qusta fase finale ogni tua singola azione ti costera\' il turno... persino controllare il tuo zaino...%s",COL_GREEN,COL_WHITE);
            
            cleanbuffer();
            finalturns++;
        }
        clr;
        do{
            bocheck1=0;
            bocheck=0;
            clr;
            printf("Questo è il tuo turno numero: %s%d%s\nNumero degli alieni uccisi: %s%d%s\nGiocatore: %s%s%s", COL_YELLOW,turn->turn,COL_WHITE,COL_RED,turn->kills,COL_WHITE,COL_BLUE,turn->name,COL_WHITE);
            Printstatus();
            printf("\n\nCome vuoi procedere? [pistola, lanciarazzi, usa oggetto, costruisci torre, zaino, esci]: %s", COL_BLUE);
            fgets(scelta, 50, stdin);
            printf("%s", COL_WHITE);
            strtolower(scelta);
        }while(strncmp(scelta, "p", 1)!=0 && strncmp(scelta, "l", 1)!=0 && strncmp(scelta, "c", 1)!=0 && strncmp(scelta, "u", 1)!=0 && strncmp(scelta, "e", 1)!=0 && strncmp(scelta, "z", 1)!=0);

        switch(scelta[0]){
            case 'z':
                clr;
                printf("Zaino:\n\n");
                for(int i=0; i<4; i++){
                    switch(turn->zaino[i]){
                        case 0:
                            printf("%d) Empty\n", i);
                            break;
                        case 1:
                            printf("%d) Medikit\n", i);
                            break;
                        case 2:
                            printf("%d) Potion\n", i);
                            break;
                        case 3:
                            printf("%d) Materials\n", i);
                            break;
                        case 4:
                            printf("%d) Rocket launcher ammo\n", i);
                            break;
                    }
                }
                break;
            case 'p':
                clr;
                x=prob;
                temp=enemy->LTower;
                if(temp!=NULL) pshoot=(50+turn->kills*10-temp->num*10);
                else pshoot=(50+turn->kills*10);
                if(x<pshoot){
                    printf("COLPITO!!!!!!!!!!!!");
                    switch(enemy->state){
                        case vulnerable:
                            enemy->state=dead;
                            printf("\nL'AVVERSARIO E\' MORTO!");
                            break;
                        case only_life:
                            enemy->state=vulnerable;
                            break;
                        case only_shield:
                            enemy->state=vulnerable;
                            break;
                        case life_shield:
                            enemy->state=only_life;
                            break;
                    }
                }
                else{
                    printf("MANCATO!!!!!!!!!!!!!\n");
                }
                break;
            case 'l':
                for(int i=0; i<4; i++){
                    if(turn->zaino[i]==rocket_launcher_bullet){
                        bocheck=1;
                        turn->zaino[i]=no_objects;
                        break;
                    }

                }
                if (bocheck==1){
                    deletetower(enemy, &directlauncher);
                    if(directlauncher==1){
                        x=prob;
                        if(x<20){
                            printf("Hai colpito il tuo nemico uccidendolo istantaneamente!!");
                            enemy->state=dead;
                        }
                        else printf("purtroppo hai mancato il bersaglio.....");
                    }

                }
                else{
                printf("Occhio!!! non hai munizioni! hai perso il tuo turno.");
                }
                break;
            case 'u':
                useobject();
                break;
            case 'e':
                exit(EXIT_SUCCESS);
                break;
            case 'c':
                for(int i=0; i<4; i++){
                    if(turn->zaino[i]==materials){
                        bocheck1=1;
                        turn->zaino[i]=no_objects;
                        break;
                    }
                }
                if(bocheck1==1) addtower();
                else{
                    printf("Occhio!!! non hai materiali! hai perso il tuo turno.");
                }
                break;
        }

        
        cleanbuffer();
        clr;

        if(turn->state!=dead){
            if (*randturn==0){
                turn=&Ninja;
                *randturn=1;
                enemy=&Ciccio;
            }
            else{
                turn=&Ciccio;
                *randturn=0;
                enemy=&Ninja;
            }
        }
        clr;
    }while(Ciccio.state!=dead && Ninja.state!=dead);
}

static void addtower(void){
    Tw *Newtower= (Tw*) malloc(sizeof(Tw));
    Tw *temp=NULL;
    Newtower->pnext=NULL;

    clr;
    printf("Hai costruito un piano della tua torre!");
    
    cleanbuffer();
    clr;

    if(turn->FTower==NULL){
        turn->FTower=Newtower;
        turn->LTower=Newtower;
        Newtower->num=1;
    }
    else{
        temp=turn->LTower;
        turn->LTower=Newtower;
        Newtower->num=temp->num++;
    }
}

static void deletetower(Pg_t *enemy, bool *l){
    Tw *temp=NULL;

    if(enemy->FTower==enemy->LTower && enemy->FTower != NULL){
        free(enemy->FTower);
        enemy->FTower=NULL;
        enemy->LTower=NULL;
        printf("Hai completamente distrutto la torre nemica!");
        cleanbuffer();
    }
    else{
        if(enemy->FTower==NULL){
            clr;
            printf("Il nemico non ha torri da abbattere!\n");
            printf("Provi a colpirlo con un colpo di lanciarazzi!");
            
            cleanbuffer();
            *l=1;
        }
        else{
            printf("Hai distrutto un piano della torre del nemico!!!\n");
            
            cleanbuffer();
            temp=enemy->FTower;
            do{
                temp=temp->pnext;
            }while(temp->pnext!=enemy->LTower);
            free(temp->pnext);
            temp->pnext=NULL;
            enemy->LTower=temp;
        }
    }

}

static void Salva(void){
    char str[30], dir[30]="./save/";
    printf("\n\nInserisci il nome del file di salvataggio: %s",COL_BLUE);
    scanf("%s",str);
    cleanbuffer();
    strcat(str, ".bin");
    strcat(dir, str);
    printf("%s",COL_WHITE);
    FILE *save=fopen(dir,"wb");
    for(int i=0; i<ncell; i++){
        for(int j=0;j<ncell; j++){
            fwrite(chessboard+(i*ncell+j), sizeof(Cell_t),1, save);
        }
    }
    fclose(save);
}

static void Carica(void){

    int x;
    char str[30], dir[30]="./save/";
    printf("\n\nInserisci il nome del file di salvataggio da caricare: %s",COL_BLUE);
    scanf("%s",str);
    cleanbuffer();
    strcat(dir, str);
    strcat(dir, ".bin");
    printf("%s",COL_WHITE);
    FILE *save;
    if((save=fopen(dir,"rb"))==NULL){
        printf("Impossibile aprire il file %s", str);
        
        cleanbuffer();
    }
    else{
        if(chessboard!=NULL){
            chessboard=NULL;
        }

        chessboard=malloc(sizeof(Cell_t));

        Cell_t appo;

        while(1){
            fread(&appo,sizeof(Cell_t),1,save);
            if(feof(save)) break;
            *(chessboard + ncell) = appo;
            chessboard=realloc(chessboard,sizeof(Cell_t)*((++ncell)+1)); 
        }
        ncell=sqrt(ncell);
        for(int i=0; i<ncell; i++){ 
            for(int j=0;j<ncell; j++){
                fread(chessboard+(i*ncell+j), sizeof(Cell_t),1, save);
            }
        }
        for(int i=0; i<ncell; i++){ 
            for(int j=0;j<ncell; j++){
                chessboard[i*ncell +j].pg=0;
            }
        }
        do{
        x=rand()%ncell;
        Ciccio.x=x;
        x=rand()%ncell;
        Ciccio.y=x;
        chessboard[Ciccio.x*ncell+Ciccio.y].pg=1;
        x=rand()%ncell;
        Ninja.x=x;
        x=rand()%ncell;
        Ninja.y=x;
        chessboard[Ninja.x*ncell+Ninja.y].pg=1;
        }while(Ciccio.x==Ninja.x && Ciccio.y==Ninja.y);
        NameAssignment(&Ninja, "Ninja");
        NameAssignment(&Ciccio, "Ciccio");
        Ciccio.state=life_shield;
        Ninja.state=life_shield;
        for(int i=0; i<4; i++){
                Ciccio.zaino[i]=no_objects;
        }
        for(int i=0; i<4; i++){
                Ninja.zaino[i]=no_objects;
        }
        Ciccio.FTower=NULL;
        Ciccio.LTower=NULL;
        Ninja.FTower=NULL;
        Ninja.LTower=NULL;
        fclose(save);
    }
}

static void cleanlists(Tw *temp){
    if(temp==NULL) return;
    else{
        if(temp->pnext==NULL) return;
        else cleanlists(temp->pnext);
        free(temp);
    }
}

void cleanplayer(void){
        cleanlists(Ciccio.FTower);
        cleanlists(Ninja.FTower);
        Ciccio.state=life_shield;
        Ninja.state=life_shield;
}

void cleanchess(void){
    free(chessboard);
}

static void cleanbuffer(){
    while(getchar()!='\n');
}