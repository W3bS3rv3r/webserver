#include "Socket/Socket.hpp"
#include <unistd.h>
#include <iostream>

int	main(int argc, char *argv[]) {

    const char* user_name = getlogin();

	if (!user_name) {
        std::cerr << "Error getting username" << std::endl;
        return (1);
    }

	std::string path = "/home/";
    path += user_name;
    path += "/webserver";

	std::cout << "Root path: " << path << std::endl;

	Socket	sckt(4242, path);
	std::string	request;

	sckt.listen();
	while (true)
		sckt.handleRequest();
	if (argc > 2)
		return (1);
	(void)argv;
	return (0);
}
