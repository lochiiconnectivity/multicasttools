/*
 * listener.c -- joins a multicast group and echoes all data it receives from
 *		the group to its stdout...
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 256

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, nbytes, addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];
	char *str;

	printf("LISTEN [Interface addr] [<IP addr>]\n");
	if (argc > 3 || argc < 2)
		return 1;
	if (argc > 2)
		str = argv[2];
	 else
		str = HELLO_GROUP;

	/* create what looks like an ordinary UDP socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/* set up receive address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(HELLO_PORT);

	/* bind to receive address */
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr(str);
	mreq.imr_interface.s_addr = inet_addr(argv[1]);
	if (setsockopt
	    (fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	printf("Interface address %s.\n", inet_ntoa(mreq.imr_interface));
	printf("Multicast address %s.\n", inet_ntoa(mreq.imr_multiaddr));

	/* now just enter a read-print loop */
	while (1) {
		addrlen = sizeof(addr);
		if ((nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0,
				       (struct sockaddr *) &addr,
				       &addrlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		puts(msgbuf);
	}
}
