#ifndef _TCPACCEPTOR_H_
#define _TCPACCEPTOR_H_

class TCPStream;

class TCPAcceptor {

public:
	int start();
	TCPStream * accept();
};

#endif

