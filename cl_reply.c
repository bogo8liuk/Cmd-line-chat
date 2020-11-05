#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cl_utils.h>

static int accept_request(int domain, int type, int protocol) {
	
}

int main(int argc, char *argv[]) {
	if (2 != argc) {
		bad_exit("Usage: %s port_number", argv[0]);
	}

	int sockfd;
}
