#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cl_utils.h>

static int accept_request(struct sockaddr *address, int domain, int type, int protocol) {
	int welcomingfd, sockfd;

	if ((welcomingfd = socket(domain, type, protocol)) < 0) {
		bad_exit("Socket creation error\n");
	}

	if (bind(welcomingfd, address, sizeof(struct sockaddr)) < 0) {
		bad_exit("Server binding error\n");
	}

	if (listen(welcomingfd, 1) < 0) {
		bad_exit("Server listening error\n");
	}

	if ((sockfd = accept(sockfd, address, sizeof(struct sockaddr))) < 0) {
		bad_exit("Server unable to accept requests error");
	}

	close(welcomingfd);
	return sockfd;
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number", argv[0]);
	}

	int sockfd;
	struct cl_init_struct *init = cl_init(argv[1], argv[2], AF_INET, SOCK_STREAM, NO_PROTOCOL);

	sockfd = accept_request((struct sockaddr *) &init->entry, init->domain, init->type, init->protocol);
	talk(sockfd);
	close(sockfd);
}
