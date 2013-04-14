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
	struct ip_mreq_source mreq;
	char msgbuf[MSGBUFSIZE];
	char *str;
	char *str2;

	printf("LISTEN [Interface addr] [<IP addr>] [<SRC addr>]\n");
	if (argc > 4 || argc < 2)
		return 1;
	else if(argc > 3)
		str2 = argv[3];

	if(argc > 2)
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

	printf("Interface address %s.\n", argv[1]);
	printf("Multicast address %s.\n", str);
	printf("Source address %s.\n", argv[3]);

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr(str);
	mreq.imr_sourceaddr.s_addr = inet_addr(argv[3]);
	mreq.imr_interface.s_addr = inet_addr(argv[1]);
	if (setsockopt
	    (fd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	printf("Interface address %s.\n", inet_ntoa(mreq.imr_interface));
	printf("Multicast address %s.\n", inet_ntoa(mreq.imr_multiaddr));
	printf("Source address %s.\n", inet_ntoa(mreq.imr_sourceaddr));

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
