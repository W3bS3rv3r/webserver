#include "parser.hpp"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>

std::pair<unsigned short, VirtualServer>	getVServer(std::fstream& file) {
	VirtualServer	vserver;
	unsigned short	port;
	std::string		buff;
	int				status;

	while(std::getline(file >> std::ws, buff)) {
		if (buff == "}")
			return (std::make_pair(port, vserver));
		try {
			status = vserver.interpretAttribute(buff, file);
		}
		catch (const std::exception& e) {
			if (!dynamic_cast<const Location::NoHomeException*>(&e))
				std::cerr << "at: '" << buff << "'" << std::endl;
			throw ;
		}
		if (status)
			port = static_cast<unsigned int>(status);
	}
	std::cerr << "missing final '}'" << std::endl;
	throw InvalidSyntaxException();
}

const char*	InvalidSyntaxException::what(void) const throw() {
	return ("Invalid syntax on config file");
}

Location	getLocation(std::fstream& file, std::string path) {
	Location		location(path);
	std::string		buff;

	while(std::getline(file >> std::ws, buff)) {
		if (buff == "}")
			break ;
		location.interpretAttribute(buff);
	}
	if (!location.checkIntegrity())
		throw InvalidSyntaxException();
	return (location);
}
