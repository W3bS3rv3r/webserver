#include "Socket/Socket.hpp"

int	main(int argc, char *argv[]) {
	Socket	sckt(4242, "/home/gcorreia/webserver");
	std::string	request;

	sckt.listen();
	while (true)
		sckt.handleRequest();
	if (argc > 2)
		return (1);
	(void)argv;
	return (0);
}
