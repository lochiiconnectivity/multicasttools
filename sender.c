/*
 * sender.c -- multicasts "hello, world!" to a multicast group once a second
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd;
	char *message = "Hello, World - keep the message longer than sixty-four bytes, to avoid any padding!";
	char *str;

	printf("SENDER [<IP addr>]\n");
	if (argc > 2)
		return 1;
	if (argc > 1)
		str = argv[1];
	 else
		str = HELLO_GROUP;
	/* create what looks like an ordinary UDP socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	{
		unsigned char loop;
		unsigned char ttl;
		struct in_addr out_addr;

		loop = 0;
		setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,
			       	sizeof(loop));

		ttl = 200 ;
		setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL,
				&ttl, sizeof(ttl));
		/* set up source address */
		out_addr.s_addr = inet_addr("172.16.2.3");
		setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &out_addr,
				sizeof(out_addr));
	}

	/* set up destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(str);
	addr.sin_port = htons(HELLO_PORT);
	printf("using address %s.\n", inet_ntoa(addr.sin_addr));

	/* now just sendto() our destination! */
	while (1) {
		if (sendto
		    (fd, message, strlen(message) + 1, 0,
		     (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("sendto");
			exit(1);
		}
		sleep(1);
	}
}
