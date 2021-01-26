#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <cl_utils.h>
#include <termios.h>

struct socket_lock {
	int sockfd;
	pthread_mutex_t *mutex;
};

static void *send_from_stdin(void *args) {
	socket_lock *pair = (struct socket_lock *) args;
	int sockfd = pair -> sockfd;
	pthread_mutex_t *echo_mutex = pair -> mutex;

	struct termios origin_attrs;

	if (0 > tcgetattr(STDIN_FILENO, &origin_attrs)) {
		bad_exit("Error on getting terminal attributes");
	}

	while (1) {
		int c = getc(stdin);

		switch (c) {
			case 'w':
				break;

			case 'e':
				break;

			case EOF:
				break;

			default:
				break;
		}
	}
}

static void *receive_to_stdout(void *args) {

}

// TODO: check the return value of the two threads
void talk(int sockfd) {
	pthread_t msg_writer, msg_printer;
	pthread_mutex_t echo_mutex;
	void *rval;

	struct socket_lock *pair = {
		.sockfd = sockfd,
		.mutex = &echo_mutex
	};

	if (0 != pthread_create(&msg_writer, NULL, send_from_stdin, (void *) pair)) {
		bad_exit("Error on creating the message writer thread");
	}

	if (0 != pthread_create(&msg_printer, NULL, receive_to_stdout, (void *) pair)) {
		bad_exit("Error on creating the message printer thread");
	}

	if (0 != pthread_join(msg_writer, &rval)) {
		bad_exit("Error on waiting for writer thread");
	}

	if (0 != pthread_join(msg_printer, &rval)) {
		bad_exit("Error on waiting for printer thread");
	}
}
