#include "Socket/Socket.hpp"

int	main(int argc, char *argv[]) {
	Socket	sckt(4242);
	std::string	request;

	sckt.listen();
	while (1)
		sckt.handleRequest();
	if (argc > 2)
		return (1);
	(void)argv;
	return (0);
}
