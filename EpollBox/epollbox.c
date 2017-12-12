#include "epollbox.h"

int ebInit()
{
	server = (ebChannel*)malloc(sizeof(ebChannel));
	epollFd = epoll_create(1024);
	return 0;
}

int ebStart(unsigned short _port,const char* _ip,int _backLog,void(*_cb)(ebChannel* _c))
{	
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	ebSetNoneBlocked(serverSocket);//serversocket非阻塞
	//start TCPserver
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(_ip);
	addr.sin_port = htons(_port);
	int ret = bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		perror("BIND ERROR!");
		return -1;
	}
	listen(serverSocket, _backLog);

	server->cb = _cb;
	server->fd = serverSocket;

	ebAddPoll(server);//将server加入epoll池

	return 0;
}

int ebAddPoll(ebChannel* _ebc)
{
	struct epoll_event ev;
	ev.data.ptr = _ebc;
	ev.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, _ebc->fd, &ev);
	return 0;
}

int ebRun()
{
	struct epoll_event ev[32];
	while (1)
	{
		int ret = epoll_wait(epollFd, ev, 32, 5000);
		if (ret == 0)continue;
		if (ret < 0 && errno == EINTR) continue;
		if (ret < 0) return -1;
		int i = 0;
		for (i = 0; i < ret; ++i)
		{
			ebChannel* c = (ebChannel*)ev[i].data.ptr;
			c->cb(c);
		}
	}
	return 0;
}

ebChannel* ebCreatChannel(int _fd, void(*_cb)(ebChannel*))
{
	ebChannel* c = (ebChannel*)malloc(sizeof(ebChannel));
	c->fd = _fd;
	c->cb = _cb;
	return c;
}

int ebDestoryChannel(ebChannel* _c)
{
	if (_c == NULL)return 0;
	close(_c->fd);
	free(_c);
}
