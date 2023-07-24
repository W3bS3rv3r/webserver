#include "Server/Server.hpp"
#include <exception>
#include <iostream>

int	main(int argc, char *argv[]) {

	Server	server;
	try {
		server.init("PLACEHOLDER" /* argv[1] in the future */);
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	try {
		server.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (2);
	}
	if (argc > 2)
		return (1);
	(void)argv;
	return (0);
}
