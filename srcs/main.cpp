#include "Server/Server.hpp"
#include <exception>
#include <iostream>

int	main(int argc, char *argv[]) {

	if (argc != 2) {
		std::cerr << "Invalid cmd line argument, please run the progam ";
		std::cerr << "like this: './webserver <config_file_path>'" << std::endl;
		return (1);
	}
	Server	server;
	try {
		server.init(argv[1]);
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (2);
	}
	try {
		server.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (3);
	}
}
