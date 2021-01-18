#include <pthread.h>
#include <cl_utils.h>

void *send_from_stdin(void *) {

}

void *receive_to_stdout(void *) {

}

void talk(int sockfd) {
	pthread_t msg_writer, msg_printer;
	pthread_mutex_t echo_mutex;
	void *rval;

	if (0 != pthread_create(&msg_writer, NULL, send_from_stdin, (void *) &echo_mutex)) {
		bad_exit("Error on creating the message writer thread");
	}

	if (0 != pthread_create(&msg_printer, NULL, receive_to_stdout, (void *) &echo_mutex)) {
		bad_exit("Error on creating the message printer thread");
	}

	if (0 != pthread_join(msg_writer, &rval)) {
		bad_exit("Error on waiting for writer thread");
	}

	if (0 != pthread_join(msg_printer, &rval)) {
		bad_exit("Error on waiting for printer thread");
	}
}
