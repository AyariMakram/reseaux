#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>




int tun_alloc(char *dev)
{
 	 struct ifreq ifr;
	  int fd, err;

	  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ){
    		perror("alloc tun");
    		exit(1);
 	 }

  	memset(&ifr, 0, sizeof(ifr));

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
   *        IFF_TAP   - TAP device  
   *
   *        IFF_NO_PI - Do not provide packet information  
   */ 
 	ifr.ifr_flags = IFF_NO_PI/*IFF_TUN*/; 
 	 if( *dev )
    		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

 	 if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
    		close(fd);
   		 return err;
 	 }
 	 strcpy(dev, ifr.ifr_name);
  	return fd;
}      

void input_tun0_content_into_file(int fd_src, int fd_dst){
	char buffer[1024];
	int bytes_read = read(fd_src, buffer, sizeof(buffer));
	/*if(bytes_read == 0){
		close(fd_src);
		close(fd_dst);
		exit(EXIT_FAILURE);
	}*/
	if(bytes_read < 0){
		perror("read\n");
		exit(EXIT_FAILURE);
	}
	
	//write(fd_dst, buffer,  sizeof(buffer));
	write(fd_dst, buffer,  sizeof(buffer));
}

int main (int argc, char*argv[]){
	if(argc < 2)
	{ 
		perror("Only 1 arg"); 
		exit(EXIT_FAILURE);
	}
	char * tun0 = argv[1];
	
	int fd_src = tun_alloc(tun0);
	if(fd_src < 0){
		perror("error : tun alloc");
		exit(EXIT_FAILURE);
	}
	int fd_dst = 1;
 	if(fd_dst < 0){
		perror("error test.txt");
		exit(EXIT_FAILURE);
	}	
	while(1){
		input_tun0_content_into_file(fd_src, fd_dst);
	}
	return 0;
}
