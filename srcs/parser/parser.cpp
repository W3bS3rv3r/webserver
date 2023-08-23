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
			status = vserver.interpretLine(buff);
		}
		catch (const std::exception& e) {
			std::cerr << "at line: '" << buff << "'" << std::endl;
			throw InvalidSyntaxException();
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
