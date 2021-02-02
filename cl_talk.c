#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <cl_utils.h>
#include <termios.h>
#include <limits.h>

#define USAGE_w "\t'w' to start writing a message; remember that when you are on this mode, incoming messages will not be prompted"
#define USAGE_e	"\t'e' to disconnect from the chat"
#define USAGE_h "\t'h' to prompt this help message"

#define TYPE_ERROR	-1
#define SEND_ERROR	-2
#define INSERT_MODE_MASK	(~ECHO & ~ICANON)

struct thread_args {
	int sockfd;
	pthread_t tid;
	pthread_mutex_t *mutex;
};

static int type_message(int sockfd) {
	char *str;

	int ret = scanf("> : %s", str);
	if (EOF == ret) {
		return TYPE_ERROR;
	}

	if ((ssize_t) ret > send(sockfd, str, strlen(str), 0)) {
		return SEND_ERROR;
	}

	return 0;
}

static void prompt_usage() {
	printf("==== USAGE ====\nType:\n%s\n%s\n%s\n===============", USAGE_w, USAGE_e, USAGE_h);
}

static void *send_from_stdin(void *args) {
	thread_args *cur_args = (struct thread_args *) args;
	int sockfd = cur_args -> sockfd;
	pthread_t printer_thread = cur_args -> tid;
	pthread_mutex_t *echo_mutex = cur_args -> mutex;

	struct termios origin_attrs;
	struct termios insert_attrs;

	if (0 > tcgetattr(STDIN_FILENO, &origin_attrs)) {
		close(sockfd);
		bad_exit("Error on getting terminal attributes");
	}

	memcpy(&insert_attrs, &origin_attrs, sizeof(origin_attrs));
	insert_attrs.c_lflag &= INSERT_MODE_MASK;

	if (0 > tcsetattr(STDIN_FILENO, TCSANOW, &insert_attrs)) {
		close(sockfd);
		bad_exit("Error on changing terminal mode");
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
					bad_exit("Error on typing message");
				} else if (SEND_ERROR == ret) {
					close(sockfd);
					bad_exit("Error on sending message, the connection with the other user may have been closed");
				}

				tcsetattr(STDIN_FILENO, TCSANOW, &insert_attrs);
				pthread_mutex_unlock(echo_mutex);
				break;

			case 'e':
				pthread_mutex_lock(echo_mutex);
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);

				if (0 != pthread_cancel(printer_thread)) {
					close(sockfd);
					bad_exit("Error on terminating thread");
				}

				printf("Closing chat\n");
				pthread_exit(NULL);
				break;

			case 'h':
				pthread_mutex_lock(echo_mutex);
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);
				prompt_usage();
				tcsetattr(STDIN_FILENO, TCSANOW, &insert_attrs);
				pthread_mutex_unlock(echo_mutex);
				break;

			case EOF:
				tcsetattr(STDIN_FILENO, TCSANOW, &origin_attrs);
				close(sockfd);
				bad_exit("Error on getting the command");
				break;

			default:
				break;
		}
	}
}

static void *receive_to_stdout(void *args) {
	thread_args *pair = (struct thread_args *) args;
	int sockfd = pair -> sockfd;
	pthread_mutex_t *echo_mutex = pair -> mutex;

	while (1) {
		char *str;
		ssize_t ret = recv(sockfd, str, SSIZE_MAX, 0);

		if (0 == ret) {
			continue;
		} else if (0 > ret) {
			close(sockfd);
			bad_exit("Error on receiving messages");
		}

		pthread_mutex_lock(echo_mutex);
		printf("END: %s\n", str);
		pthread_mutex_unlock(echo_mutex);
	}
}

void talk(int sockfd) {
	pthread_t msg_writer, msg_printer;
	pthread_mutex_t echo_mutex;
	void *rval;

	if (0 != pthread_mutex_init(&echo_mutex, NULL)) {
		close(sockfd);
		bad_exit("Error on initiating the mutex to write on stdout");
	}

	struct thread_args *writer_args = {
		.sockfd = sockfd,
		.tid = msg_printer,
		.mutex = &echo_mutex
	};

	struct thread_args *printer_args = {
		.sockfd = sockfd,
		.tid = msg_writer,
		.mutex = &echo_mutex
	};

	if (0 != pthread_create(&msg_writer, NULL, send_from_stdin, (void *) writer_args)) {
		close(sockfd);
		bad_exit("Error on creating the message writer thread");
	}

	if (0 != pthread_create(&msg_printer, NULL, receive_to_stdout, (void *) printer_args)) {
		close(sockfd);
		bad_exit("Error on creating the message printer thread");
	}

	if (0 != pthread_join(msg_printer, &rval)) {
		close(sockfd);
		bad_exit("Error on waiting for printer thread");
	}

	if (0 != pthread_join(msg_writer, &rval)) {
		close(sockfd);
		bad_exit("Error on waiting for writer thread");
	}
}
