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

void ext_in(char * hote, char * port, char * tun0);

int main(int argc, char *argv[]){
	/* Traitement des arguments */
	if (argc!=4) {/* erreur de syntaxe */
	printf("Usage: %s hote port\n",argv[0]);
	exit(1);
	}
	char * hote=argv[1]; /* nom d'hÃ´te du  serveur */   
	char * port=argv[2]; /* port TCP du serveur */  
	char * tun0=argv[3]; /* nom de de l'interface*/
	ext_in(hote, port, tun0);

	return EXIT_SUCCESS;
}

/*Prend en paramètre le nom d'hôte du serveur ainsi que son port TCP*/   
void ext_in(char * hote, char * port, char * tun0){
	char ip[NI_MAXHOST]; /* adresse IPv4 en notation pointÃ©e */
	struct addrinfo *resol; /* struct pour la rÃ©solution de nom */
	int s; /* descripteur de socket */

	/* RÃ©solution de l'hÃ´te */
	if ( getaddrinfo(hote,port,NULL, &resol) < 0 ){
		perror("rÃ©solution adresse");
		exit(2);
	}

	/* On extrait l'addresse IP */
	inet_ntop(AF_INET6, &(((struct sockaddr_in6*)resol->ai_addr)->sin6_addr), ip, INET6_ADDRSTRLEN);

	/* CrÃ©ation de la socket, de type TCP / IP */
	/* On ne considÃ¨re que la premiÃ¨re adresse renvoyÃ©e par getaddrinfo */
	if ((s=socket(resol->ai_family,resol->ai_socktype, resol->ai_protocol))<0) {
		perror("allocation de socket");
		exit(3);
	}
	fprintf(stderr,"le nÂ° de la socket est : %i\n",s);

	/* Connexion */
	fprintf(stderr,"Essai de connexion Ã  %s (%s) sur le port %s\n\n",hote,ip,port);
	if (connect(s,resol->ai_addr,sizeof(struct sockaddr_in6))<0) {
		perror("connexion");
		exit(4);
	}
	freeaddrinfo(resol); /* /!\ LibÃ©ration mémoire */

	/* Session */
	char buffer[1024];
	int bytes_read;
	int fd_tun = tun_alloc(tun0);
	if(system("./configure-all.sh")<0){
		perror("configure-all.sh");
		exit(5);  
	}
  	while(1) {
		/*Lit depuis le file descriptor de tun0 le contenu de tun0*/
  		bytes_read = read(fd_tun, buffer, sizeof(buffer));
		if (bytes_read > 0){
			buffer[bytes_read] = '\0';
			/*envoie le contenu de tun0 au serveur d'écoute*/
			send(s,buffer,bytes_read,0);
		} 
		else {
			break;
		}
  	}
}
