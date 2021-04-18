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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/socket.h>

#define MAX_MESSAGE_SIZE	4096

#define bad_exit(...)	do {    \
                            fprintf(stderr, __VA_ARGS__);	\
                            exit(EXIT_FAILURE); \
                        } while (0)

struct cl_init_struct {
	struct sockaddr_in *entry;
	int domain;
	int type;
	int protocol;
};

extern struct cl_init_struct *cl_init(char *const address, char *const port, int domain, int type, int protocol);

extern bool port_conversion(char *const port, uint16_t *res);

extern void talk(int sockfd);
