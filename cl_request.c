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

	sockfd = connect_request((struct sockaddr *) init -> entry, init -> domain, init -> type, init -> protocol);
	printf("Connected to %s at port %s\n", argv[1], argv[2]);
	talk(sockfd);
	close(sockfd);
}
