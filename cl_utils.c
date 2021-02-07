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
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <termios.h>
#include <pthread.h>
#include <cl_utils.h>

#define TYPE_ERROR	-1
#define SEND_ERROR	-2
#define INSERT_MODE_MASK	(~ECHO & ~ICANON)

#define USAGE_w "\t'w' to start writing a message; remember that when you are on this mode, incoming messages will not be prompted"
#define USAGE_e	"\t'e' to disconnect from the chat"
#define USAGE_h "\t'h' to prompt this help message"

struct thread_args {
	int sockfd;
	pthread_t *tid;
	pthread_mutex_t *mutex;
};

static int type_message(int sockfd) {
	char str[MAX_MESSAGE_SIZE + 1] = { 0 };

	printf("> : ");
	fflush(stdout);
	if (NULL == fgets(str, MAX_MESSAGE_SIZE, stdin)) {
		return TYPE_ERROR;
	}

	const size_t length = strlen(str);

	if (0 == length || 1 == length) {
		return 0;
	}

	if ((ssize_t) length != send(sockfd, str, length, 0)) {
		return SEND_ERROR;
	}

	return 0;
}

static void prompt_usage() {
	printf("==== USAGE ====\nType:\n%s\n%s\n%s\n===============\n", USAGE_w, USAGE_e, USAGE_h);
}

static void *send_from_stdin(void *args) {
	struct thread_args *cur_args = (struct thread_args *) args;
	int sockfd = cur_args -> sockfd;
	pthread_t printer_thread = *(cur_args -> tid);
	pthread_mutex_t *echo_mutex = cur_args -> mutex;

	struct termios origin_attrs;
	struct termios insert_attrs;

	if (0 > tcgetattr(STDIN_FILENO, &origin_attrs)) {
		close(sockfd);
		bad_exit("Error on getting terminal attributes\n");
	}

	memcpy(&insert_attrs, &origin_attrs, sizeof(origin_attrs));
	insert_attrs.c_lflag &= INSERT_MODE_MASK;

	if (0 > tcsetattr(STDIN_FILENO, TCSANOW, &insert_attrs)) {
		close(sockfd);
		bad_exit("Error on changing terminal mode\n");
	}

	while (1) {
		int c = getc(stdin);

		switch (c) {
			case 'w':
				pthread_mutex_lock(echo_mutex);
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);

				int ret = type_message(sockfd);
				if (TYPE_ERROR == ret) {
					close(sockfd);
					bad_exit("Error on typing message\n");
				} else if (SEND_ERROR == ret) {
					close(sockfd);
					bad_exit("Error on sending message, the connection with the other user may have been closed\n");
				}

				tcsetattr(STDIN_FILENO, TCSANOW, &insert_attrs);
				pthread_mutex_unlock(echo_mutex);
				break;

			case 'e':
				pthread_mutex_lock(echo_mutex);
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);

				if (0 != pthread_cancel(printer_thread)) {
					close(sockfd);
					bad_exit("Error on terminating thread\n");
				}

				printf("\nClosing chat\n");
				pthread_exit(NULL);
				break;

			case 'h':
				pthread_mutex_lock(echo_mutex);
				prompt_usage();
				pthread_mutex_unlock(echo_mutex);
				break;

			case EOF:
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);
				close(sockfd);
				bad_exit("Error on getting the command\n");
				break;

			default:
				break;
		}
	}
}

static void *receive_to_stdout(void *args) {
	struct thread_args *cur_args = (struct thread_args *) args;
	int sockfd = cur_args -> sockfd;
	pthread_mutex_t *echo_mutex = cur_args -> mutex;

	while (1) {
		char str[MAX_MESSAGE_SIZE + 1] = { 0 };
		ssize_t ret = recv(sockfd, str, MAX_MESSAGE_SIZE, 0);

		if (0 == ret) {
			continue;
		} else if (0 > ret) {
			close(sockfd);
			bad_exit("Error on receiving messages\n");
		}

		pthread_mutex_lock(echo_mutex);
		printf("\tEND: %s", str);
		pthread_mutex_unlock(echo_mutex);
	}
}

struct cl_init_struct *cl_init(char *const address, char *const port, int domain, int type, int protocol) {
	struct cl_init_struct *init = (struct cl_init_struct *) malloc(sizeof(struct cl_init_struct));
	struct sockaddr_in *entry = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

	if (inet_pton(domain, address, &(entry -> sin_addr.s_addr)) != 1) {
		bad_exit("Address conversion error\n");
	}

	if (!port_conversion(port, (uint16_t *) &(entry -> sin_port))) {
		bad_exit("Port conversion error\n");
	}

	entry -> sin_family = domain;

#ifdef __gnu_linux__
	memset(entry -> sin_zero, 0, sizeof(entry -> sin_zero));
#endif

	init -> entry = entry;
	init -> domain = domain;
	init -> type = type;
	init -> protocol = protocol;

	return init;
}

bool port_conversion(char *const port, uint16_t *res) {
	uintmax_t value;
	char *end;
	errno = 0;
	value = strtoumax(port, &end, 10);
	if (ERANGE == errno || UINT16_MAX < value)
		return false;
	else {
		*res = htons((uint16_t) value);
		return true;
	}
}

void talk(int sockfd) {
	pthread_t msg_writer, msg_printer;
	pthread_mutex_t echo_mutex;
	void *rval_printer;
	void *rval_writer;

	if (0 != pthread_mutex_init(&echo_mutex, NULL)) {
		close(sockfd);
		bad_exit("Error on initiating the mutex to write on stdout\n");
	}

	struct thread_args writer_args = {
		.sockfd = sockfd,
		.tid = &msg_printer,
		.mutex = &echo_mutex
	};

	struct thread_args printer_args = {
		.sockfd = sockfd,
		.tid = &msg_writer,
		.mutex = &echo_mutex
	};

	if (0 != pthread_create(&msg_writer, NULL, send_from_stdin, &writer_args)) {
		close(sockfd);
		bad_exit("Error on creating the message writer thread\n");
	}

	if (0 != pthread_create(&msg_printer, NULL, receive_to_stdout, &printer_args)) {
		close(sockfd);
		bad_exit("Error on creating the message printer thread\n");
	}

	if (0 != pthread_join(msg_printer, &rval_printer)) {
		close(sockfd);
		bad_exit("Error on waiting for printer thread\n");
	}

	if (0 != pthread_join(msg_writer, &rval_writer)) {
		close(sockfd);
		bad_exit("Error on waiting for writer thread");
	}
}
