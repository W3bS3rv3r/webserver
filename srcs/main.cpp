#include "Server/Server.hpp"
#include <exception>
#include <iostream>
#include <cstdlib>

int	main(int argc, char *argv[]) {
	std::string	conf_path;
	std::string home_path;

	if (argc == 1) {
        home_path = getenv("HOME");
        if (home_path.empty()) {
            std::cerr << "Error: HOME environment variable not set." << std::endl;
            return (1);
        }
        conf_path = home_path + "/w3b53rb3ru5/w3b53rb3ru5.conf";
        std::cout << "Warning: Config file missing.\n";
        std::cout << "Running webserver with default config file: " << conf_path << std::endl;
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
