#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SBUFFER 1000
#define NBUFFER 8000
#define MARGIN  1
#define SOGLIA  1

/*---strutture-----------------------------------------------*/
   /*ES: 0 a b R 4   0= numstate , a= leggo, b= scrivo, R= mossa, 4= nextstate*/
    /*lista transizioni per ogni carattere letto nello stato di appartenenza*/
    typedef struct tran_s{
        char scrivo; 
        unsigned int nextstate;
        int mossa;
        struct tran_s *next;
    }tran_t;

    /*lista letture da nastro per ogni stato*/
    typedef struct read_s{
        char leggo;
        unsigned int nd;
        struct read_s *nextleggo;
        tran_t * write;
    }read_t;
    
   /*array stati rimanda alle letture da nastro possibili*/
    typedef struct state_s{
        unsigned int acc;
        read_t *read;
    }state_t;
    
    typedef struct nastro_s{
        char*car;
        unsigned int utilizzatori,nlen;
        struct nastro_s *next;
    }nastro_t;
    
    nastro_t *elenconastri;
    char ris;
    
    /*-configurazioni-*/
    typedef struct config_s{
        unsigned int state, movecont;
        long int pos;
        int flagof;
        nastro_t *nastro;
        struct config_s *next;
    }config_t;
    
     
    
    
    
/*__________________PROTOTIPI FUNZIONI___________________*/

/*acquisizione*/
state_t * incstato(state_t*, unsigned int*,unsigned int);    
read_t * inread(state_t* ,char, unsigned int);
tran_t* intrans(read_t*,char,char,unsigned int);
state_t * acquisiscitr(state_t*, unsigned int *);
void liberatransizioni(state_t*, unsigned int * );

/*accettazione*/
void setacc(state_t*);

/*simulazione*/
char simulazione(state_t*,unsigned int);
char * allarganastro(char*,unsigned int*);
nastro_t* dupliallarganastro(nastro_t*);
read_t * listtrans(config_t *, state_t *);
config_t*rimuovi(config_t*);
int avanzaconf(config_t*, int,read_t*);
nastro_t* duplicanastro(nastro_t*);
config_t * aggiungiconfig(config_t*);
void liberatutti(config_t*);
void liberatuttinastri(nastro_t*);
void garbage(nastro_t*);
nastro_t* nastrolibero(nastro_t*);



/*______________________________________________________*/
/*______________________FUNZIONI_______________________*/
/*______________________________________________________*/

/*____acquisizione___ OK*/

/*-acquisisce tr-*/
state_t * acquisiscitr(state_t * head, unsigned int * maxstate){
    
    /*---acquisizione transizioni e creazione liste---*/
   
    char tr[3], *first,*temp_read,*temp_write,*temp_move; //variabili temporanee
    read_t *current;
    unsigned int stateA, stateB;
    
    
    
    scanf("%s",tr); //salto la riga tr
    
    while(1){
        
        scanf("%ms",&first);//leggo primi caretteri
        if(first[0]=='a'){ //ciclo infinto, se trovo a = sono ad acc e esco
            free(first);
            return head;
            //break;
        }
        
        stateA = atoi(first); // altrimenti converto in intero e salvo nell'array
        free(first);
        scanf("%ms%ms%ms%u", &temp_read, &temp_write, &temp_move, &stateB);//acq var temp
        
        if(stateA>=*maxstate){
            head = incstato(head,maxstate,stateA); //array stati tropp piccolo, incremento A
        }
        if(stateB>=*maxstate){
            head = incstato(head,maxstate,stateB); //array stati tropp piccolo, incremento B
        }     
              
        current = inread(head,*temp_read,stateA); //inserisco la lettura nell array;
        free(temp_read);
        
        intrans(current,*temp_write,*temp_move,stateB); // inserisco la transizione per carattere letto corrispondente
        
        free(temp_write);
        free(temp_move);
        
        }
     
}

/*-inserisce read-*/
read_t * inread(state_t * head, char temp, unsigned int s){
    read_t *nuovo, *p;
    if(head[s].read){// testa non vuota
        for(p=head[s].read; p->nextleggo && p->leggo != temp; p=p->nextleggo)
            ;
        if(p->leggo ==temp){
          return p;//già presente, ritorno il punto di arresto  
        }
          //non trovo il carattere lo creo
            nuovo = calloc(sizeof(read_t),1);
            nuovo->leggo=temp;
            p->nextleggo=nuovo;
            return nuovo;
        }     
    head[s].read = calloc(sizeof(read_t),1);
    head[s].read->leggo=temp;
    return head[s].read;
    
    }
    
/*-inserisce trans--*/
tran_t * intrans(read_t * head, char wr, char move, unsigned int nextst){
    tran_t *nuovo, *p;
    if(head->write){// testa non vuota
        head->nd=1;
        for(p=head->write; p->next; p=p->next)
            ;
        nuovo = calloc(sizeof(read_t),1); //aggiungo
        nuovo->scrivo=wr;
        switch(move){ //aggiorno posizione testina
            case 'R': nuovo->mossa=1;
            break;
            case 'L': nuovo->mossa=-1;
            break;
        }             
        nuovo->nextstate=nextst;
        p->next=nuovo;
        return nuovo;
        }
    nuovo = calloc(sizeof(read_t),1); //aggiungo il primo
    nuovo->scrivo=wr;
    switch(move){ //aggiorno posizione testina
            case 'R': nuovo->mossa=1;
            break;
            case 'L': nuovo->mossa=-1;
            break;
        }
    nuovo->nextstate=nextst;
    head->write=nuovo;
    return nuovo;
}

/*-incrementa l'array degli stati---OK*/
state_t * incstato(state_t * head, unsigned int * maxstate, unsigned int numst){
    unsigned int new,max;
    max=*maxstate;
    new=max+SBUFFER;  
    
    while(numst>new) {
        new=new + SBUFFER; //incremento dim maxstate
    };
    
    //realloco e inizializzo a 0
    head=realloc(head,sizeof(state_t)*new);
    memset(head+max,0,(new-max)*sizeof(state_t));
    
    *maxstate=new;
    
    return head;
    
  }

/*__accettazione__OK ins controllo overflow(?)*/
void setacc(state_t*head){
    char *stream=NULL;
    while(1){
        scanf("%ms",&stream);//leggo primi caretteri
        if(stream[0]=='m'){ //ciclo infinto, se trovo a = sono ad acc e esco
            free(stream);
            return;
        }else{
            head[atoi(stream)].acc=1;
            free(stream);
        }
    }
}

void liberatransizioni(state_t*head, unsigned int * maxss){
    state_t *p;
    read_t *l, *prevl;
    tran_t *t, *prevt;
    unsigned int i,max;
    max=*maxss;
    for(p=head, i=0; i<max; i++){
        for(l=p[i].read; l; ){
            for(t=l->write; t; ){
                prevt=t;
                t=t->next;
                free(prevt);
            }
            prevl=l;
            l=l->nextleggo;
            free(prevl);
         }
        
    }
    return;
    
}

/*____simulazione________________________________________________________*/

char * allarganastro(char* old ,unsigned int * len){
    char* new;
    unsigned int oldlen;
    oldlen= *len;
    new = malloc(sizeof(char)*(oldlen + NBUFFER));
    memset(new,'_',sizeof(char)*(oldlen + NBUFFER));
    memmove(new+(NBUFFER/2),old,oldlen*sizeof(char));
    //free(old);
    *len=oldlen+NBUFFER;
    return new;
}

read_t * listtrans(config_t * conf, state_t * liststati){
    read_t *p;
    unsigned int stato;
    char letto;
    letto=conf->nastro->car[conf->pos];
    stato= conf->state;
    for(p=liststati[stato].read; p && letto!= p->leggo;p=p->nextleggo)
                ;
    return p;  
    
}

config_t * rimuovi(config_t * conf){
    config_t *p;
    if(conf==conf->next){
        conf->nastro->utilizzatori--;
        free(conf);
        return 0; //se rimuovo l'ultima ritorno 0
    }
    for(p=conf; p->next!=conf; p=p->next)
         ;
    p->next=conf->next;                  
    conf->nastro->utilizzatori--;
    free(conf);
    return p; // altrimenti la conf immediatamente prec a quella rimossa
}
nastro_t* nastrolibero(nastro_t*n){
    for(n;n && n->utilizzatori!=0;n=n->next)
        ;
    return n;
}

nastro_t * dupliallarganastro(nastro_t *nas){
    nas->utilizzatori--;
    nas=duplicanastro(nas);
    nas->car=allarganastro(nas->car,&nas->nlen);
    return nas;
}

nastro_t* duplicanastro(nastro_t * old){
    char *new;
    nastro_t *newnastro;
    if(!(newnastro = nastrolibero(elenconastri))){
        newnastro = malloc(sizeof(nastro_t));
        newnastro->next=old->next;
        old->next=newnastro;
        
    }else{
        free(newnastro->car);
    }
    new = malloc(sizeof(char)*(old->nlen));
    memcpy (new, old->car, sizeof(char)*(old->nlen));
    newnastro->car=new;
    newnastro->utilizzatori=1;
    newnastro->nlen=old->nlen;
    
    return newnastro;
}

/*return 1 se ok, se supero max mosse ritorno: 0 se era l'ultima conf, -1 se ha sup */
config_t * avanzaconfig(config_t* conf,unsigned int max, tran_t *t){
    nastro_t * old;
    conf->movecont++;//aggiorno numero mosse
    if(conf->movecont>max ||
        ((t->nextstate==conf->state)&&(t->scrivo==conf->nastro->car[conf->pos])&&!(t->mossa))
        ||  ((t->nextstate==conf->state)&&(conf->flagof==1)&& conf->nastro->car[conf->pos]=='_'&&(t->mossa==1))
        ||  ((t->nextstate==conf->state)&&(conf->flagof==-1)&& conf->nastro->car[conf->pos]=='_'&&(t->mossa==-1))
             ){
        ris='U';
        return rimuovi(conf);//0 se rimuovo l'ultima altrmenti la precedente
    }
    conf->state=t->nextstate; //avanzo lo stato
    
    if(conf->nastro->car[conf->pos]!=t->scrivo){ //COPY on WRITE
        old=conf->nastro;
        conf->nastro=duplicanastro(old);// creo nuovo nastro
        old->utilizzatori--;
    } 
    conf->nastro->car[conf->pos]=t->scrivo; // lo modifico                                        
    (conf->pos)=(conf->pos)+(t->mossa); // aggiorno testina
    return conf;
}

config_t* aggiungiconfig(config_t*conf){
    config_t *nuova;
    nuova = malloc(sizeof(config_t));
    memcpy(nuova, conf ,sizeof(config_t));
    conf->nastro->utilizzatori++;
    conf->next=nuova;
    return nuova;
}

void liberatutti(config_t*conf){
    config_t *p, *old;
    for(p=conf->next, conf->next=NULL ;p  ;){
        old=p;
        p=p->next;
        free(old);
    }
    return;    
    
}

void liberatuttinastri(nastro_t* head){
    nastro_t *p, *old;
    for(p=head ;p  ;){
        free(p->car);
        old=p;
        p=p->next;
        free(old);
    }
    return;  
}

void garbage(nastro_t*head){
    nastro_t *p, *old, *prev;
    
    for(p=head->next, prev=head ;p  ;){
        if(p->utilizzatori==0){
        free(p->car);
        old=p;
        p=p->next;
        free(old);
        prev->next=p;
        }else{
            p=p->next;
            prev=prev->next;
        }
    }
    return;
}

char simulazione(state_t*head,unsigned int max){
    char *nast=NULL;
    unsigned int lunghezza;
    int ret, cont;
    nastro_t *ns;
    read_t *p;
    config_t *conf;
    tran_t *t; 
    //ris='U';
    cont=0;
    ret = scanf("%ms",&nast);//acquisisce nastro
    if(ret<=0){//controlla la fine del file
        return ret;
    }
            
    
    conf=calloc(sizeof(config_t),1);//alloco prima configuraizone MT
    ns=calloc(sizeof(nastro_t),1);
    elenconastri=ns;
    conf->nastro=ns;
    conf->nastro->nlen=strlen(nast);
    ns->car=allarganastro(nast,&conf->nastro->nlen);
    free(nast);
    ns->utilizzatori=1;
    ns->next=NULL;
    conf->pos=NBUFFER/2;    
    conf->next=conf;
    ris='_';       
    
    for( ; conf ;conf=conf->next){ //da ogni configurazione passo nella successiva                 
        
        /*controllo se sono in stato finale*/
        if(head[conf->state].acc){
            ris='1';
            break;
        }
        
        /*preparo la transizione*/
        if(!(p = listtrans(conf,head))){ //return NULL se non trova...
            if(!(conf=rimuovi(conf))){ //... rimuovo. se NULL termino
                if(ris=='_')
                      ris='0';
                break;
            }  // altrimenti passo alla succ.
            continue;
        }
        
        if(((conf->pos+MARGIN) > conf->nastro->nlen )){
            conf->nastro = dupliallarganastro(conf->nastro);
            conf->pos=conf->pos + NBUFFER/2;
            conf->flagof=1;
        }else if(((conf->pos)-MARGIN) < 0){
            conf->nastro = dupliallarganastro(conf->nastro);
            conf->pos=conf->pos + NBUFFER/2;
            conf->flagof=-1;
            
        }
        
      /*  /*controllo overflow nastro
        if(((conf->pos+MARGIN) > conf->nastro->nlen )|| ((conf->pos)-MARGIN) <= 0){
            conf->nastro = dupliallarganastro(conf->nastro);
            conf->pos=conf->pos + NBUFFER/2;
            conf->flagof=1;
        }*/
        
        /*eseguo le modifiche alla configurazione*/
        for(t=p->write; t->next; t=t->next, conf=conf->next){
            aggiungiconfig(conf);
            conf=avanzaconfig(conf,max,t);//non potrà mai essere 0,
            conf->flagof=0;
        }
        if(!(conf=avanzaconfig(conf,max,t)))//se sfora ed era l'ultima 0 altrimenti inidirizzo di se stessa o della conf prec
                break;
        conf->flagof=0;
        
        for(cont++; cont>SOGLIA; cont=0) //eseguo alternato;
            garbage(elenconastri);
    }
printf("%c\n",ris); 
liberatuttinastri(elenconastri);
if(conf)
    liberatutti(conf);
return ret;      
}
    
    


/*----------------------------------------------------------------------*/




int main(int argc, char*argv[]){
    freopen("pubblico.txt","r",stdin);
    state_t *head=NULL;
    unsigned int maxmove,maxstati;
    int ris;
    char run[4];
    maxmove=0;
    maxstati= SBUFFER;
    head = calloc(SBUFFER,sizeof(state_t));
    
    /*acquisizione*/
    head = acquisiscitr(head, &maxstati);
    /*salvo stati di acc*/
    setacc(head);
    /*salvo maxmove*/
    scanf("%u",&maxmove);
    scanf("%s",run);
     
    /*SIMULAZIONE*/
    do{
    ris=simulazione(head,maxmove);
    }while(ris>0);
    
    liberatransizioni(head,&maxstati);
    
    free(head);
    return 0;
    
}
