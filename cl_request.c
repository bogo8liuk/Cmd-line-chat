#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cl_utils.h>
#include <cl_talk.h>

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

	return(-1);
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number\n", argv[0]);
	}

	struct sockaddr_in entry;
	int sockfd;
	int domain = AF_INET;
	int type = SOCK_STREAM;
	int protocol = 0;

	if (inet_pton(AF_INET, argv[1], &entry.sin_addr) != 1) {
		bad_exit("Address conversion error\n");
	}

	if (!port_conversion(argv[2], (uint16_t *) &entry.sin_port)) {
		bad_exit("Port conversion error\n");
	}

	entry.sin_family = AF_INET;

	printf("%d\n", (int) entry.sin_port);
	sockfd = connect_request((struct sockaddr *) &entry, domain, type, protocol);
}
