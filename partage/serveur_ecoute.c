/* echo / serveur simpliste
   Master Informatique 2012 -- UniversitÃ© Aix-Marseille  
   Emmanuel Godard
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "interface.h"

/* taille maximale des lignes */
#define MAXLIGNE 80
#define CIAO "Au revoir ...\n"

//void echo(int f, char* hote, char* port);
void ext_out();
void traitement_out(int fd);
int main(int argc, char *argv[])
{
	if (argc!=2) {
		printf("Usage: %s port\n",argv[0]);
		exit(1);
	}
	fprintf(stderr,"Ecoute sur le port %s\n",argv[1]);
	ext_out(argv[1]);
	return EXIT_SUCCESS;
}

void ext_out(char * port){
  int s,n; /* descripteurs de socket */
  int len,on; /* utilitaires divers */
  struct addrinfo * resol; /* rÃ©solution */
  struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
                           PF_INET6,SOCK_STREAM,0, /* IP mode connectÃ© */
                           0,NULL,NULL,NULL};
  struct sockaddr_in6 client; /* adresse de socket du client */
  int err; /* code d'erreur */
  
  err = getaddrinfo(NULL,port,&indic,&resol); 
  if (err<0){
    fprintf(stderr,"RÃ©solution: %s\n",gai_strerror(err));
    exit(2);
  }

  /* CrÃ©ation de la socket, de type TCP / IP */
  if ((s=socket(resol->ai_family,resol->ai_socktype,resol->ai_protocol))<0) {
    perror("allocation de socket");
    exit(3);
  }
  fprintf(stderr,"le nÂ° de la socket est : %i\n",s);

  /* On rend le port rÃ©utilisable rapidement /!\ */
  on = 1;
  if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0) {
    perror("option socket");
    exit(4);
  }
  fprintf(stderr,"Option(s) OK!\n");

  /* Association de la socket s Ã  l'adresse obtenue par rÃ©solution */
  if (bind(s,resol->ai_addr,sizeof(struct sockaddr_in6))<0) {
    perror("bind");
    exit(5);
  }
  freeaddrinfo(resol); /* /!\ LibÃ©ration mÃ©moire */
  fprintf(stderr,"bind!\n");

  /* la socket est prÃªte Ã  recevoir */
  if (listen(s,SOMAXCONN)<0) {
    perror("listen");
    exit(6);
  }
  fprintf(stderr,"listen!\n");

  while(1) {
    /* attendre et gÃ©rer indéfiniment les connexions entrantes */
    len=sizeof(struct sockaddr_in6);
    if( (n=accept(s,(struct sockaddr *)&client,(socklen_t*)&len)) < 0 ) {
      perror("accept");
      exit(7);
    }
    /* Nom réseau du client */
    char hotec[NI_MAXHOST];  char portc[NI_MAXSERV];
    err = getnameinfo((struct sockaddr*)&client,len,hotec,NI_MAXHOST,portc,NI_MAXSERV,0);
    if (err < 0 ){
      fprintf(stderr,"rÃ©solution client (%i): %s\n",n,gai_strerror(err));
    }else{ 
      fprintf(stderr,"accept! (%i) ip=%s port=%s\n",n,hotec,portc);
    }
    /* traitement */
    traitement_out(n);
  }
}

void traitement_out(int fd){
	ssize_t lu; /* nb d'octets reÃ§us */
	char tampon[MAXLIGNE+1]; /* tampons pour les communications */ 

	while(1){ /* reçoit les données*/
		lu = recv(fd,tampon,MAXLIGNE,0);
    		if (lu > 0){
			tampon[lu] = '\0';
			/*écrit le contenu sur la sortie standard*/
			write(1, tampon,  lu);
		} 
      		else {
			break;
      		}
	}
}
