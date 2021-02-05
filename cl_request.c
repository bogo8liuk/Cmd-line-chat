#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cl_utils.h>

static int connect_request(const struct sockaddr *addr, int domain, int type, int protocol) {
	int sockfd;

	while (true) {
		if ((sockfd = socket(domain, type, protocol)) < 0) {
			bad_exit("Socket creation error%s\n", strerror(errno));
		}

		if (connect(sockfd, addr, sizeof(struct sockaddr)) == 0)
			return sockfd;

		close(sockfd);
	}

	return -1;
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number\n", argv[0]);
	}

	int sockfd;
	struct cl_init_struct *init = cl_init(argv[1], argv[2], AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockfd = connect_request((struct sockaddr *) &init -> entry, init -> domain, init -> type, init -> protocol);
	printf("Connected to %s at port %s\n", argv[1], argv[2]);
	talk(sockfd);
	close(sockfd);
}
