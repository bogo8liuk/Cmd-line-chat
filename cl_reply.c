/*
Cmd-line-chat

MIT License

Copyright (c) 2020 bogo8liuk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <unistd.h>
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

	if ((sockfd = accept(welcomingfd, NULL, NULL)) < 0) {
		bad_exit("Server unable to accept requests error\n");
	}

	close(welcomingfd);
	return sockfd;
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number\n", argv[0]);
	}

	int sockfd;
	struct cl_init_struct *init = cl_init(argv[1], argv[2], AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockfd = accept_request((struct sockaddr *) init -> entry, init -> domain, init -> type, init -> protocol);
	talk(sockfd);
	close(sockfd);
}
