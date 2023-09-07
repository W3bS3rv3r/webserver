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
			break ;
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
	return (std::make_pair(port, vserver));
}

bool	validFieldName(std::string field) {
	if (field == "port" || field == "root" || field == "cgi_extension" ||
		field == "server_name")
	{
		return true;
	}
	return false;
}

const char*	InvalidSyntaxException::what(void) const throw() {
	return ("Invalid syntax on config file");
}

Location	getLocation(std::fstream& file) {
	Location		location;
	std::string		buff;

	while(std::getline(file >> std::ws, buff)) {
		if (buff == "}")
			break ;
		location.interpretAttribute(buff);
	}
	if (!location.checkIntegrity())
		throw std::exception();
	return (location);
}
