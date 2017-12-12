#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>

typedef struct _ebChannel
{
	int fd;
	void(*cb)(struct _ebChannel* c);
}ebChannel;

ebChannel* server;
int epollFd = 0;


int ebInit();

int ebStart(unsigned short _port, const char* _ip, int _backLog, void(*_cb)(ebChannel* _c));

int ebAddPoll(ebChannel* _ebc);

int ebRun();

ebChannel* ebCreatChannel(int _fd, void(*_cb)(ebChannel*));

int ebDestoryChannel(ebChannel* _c);

static int ebSetNoneBlocked(int _fd)
{
	int flag = fcntl(_fd, F_GETFL);
	flag |= O_NONBLOCK;
	int ret = fcntl(_fd, F_SETFL, flag);
	return 0;
}
