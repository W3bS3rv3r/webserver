#include "Server/Server.hpp"
#include <exception>
#include <iostream>

#define DEFAULT_CONFIG "/etc/w3b53rb3ru5/w3b53rb3ru5.conf"

int	main(int argc, char *argv[]) {
	std::string	conf_path;

	if (argc == 1) {
		std::cout << "Warning: Config file missing.\n";
		std::cout << "Running webserver with default config file: " << DEFAULT_CONFIG << std::endl;
		conf_path = DEFAULT_CONFIG;
	}
	else if (argc > 2) {
		std::cerr << "Invalid cmd line argument, please run the progam ";
		std::cerr << "like this: './webserver <config_file_path>'" << std::endl;
		return (1);
	}
	else {
		conf_path = argv[1];
	}
	Server	server;
	try {
		server.init(conf_path);
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
