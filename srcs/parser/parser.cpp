#include "parser.hpp"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>

std::pair<unsigned short, VirtualServer>	getVServer(std::fstream& file) {
	std::map<std::string, std::string>	parameters = readParameters(file);
	unsigned short	port;

	if (parameters.find("port") != parameters.end()) {
		std::stringstream	str(parameters["port"]);
		str >> port;
	}
	else
		port = 80;
	return (std::make_pair(port, VirtualServer(parameters)));
}

std::map<std::string, std::string>	readParameters(std::fstream& file) {
	std::string							buff, field, content;
	std::stringstream					stream;
	std::map<std::string, std::string>	parameters;

	while(std::getline(file >> std::ws, buff)) {
		if (buff == "}")
			break ;
		stream << buff;
		stream >> field;
		if (!validFieldName(field)) {
			std::cerr << "at line: '" << buff << "'" << std::endl;
			throw InvalidSyntaxException();
		}
		stream >> content;
		stream >> std::ws;
		if (!stream.eof() || content.empty()) {
			std::cerr << "at line: '" << buff << "'" << std::endl;
			throw InvalidSyntaxException();
		}
		parameters.insert(std::make_pair(field, content));
		field.clear();
		content.clear();
		stream.clear();
	}
	return (parameters);
}

bool	validFieldName(std::string field) {
	if (field == "port" || field == "location" || field == "cgi_extension" ||
		field == "server_name")
	{
		return true;
	}
	return false;
}

const char*	InvalidSyntaxException::what(void) const throw() {
	return ("Invalid syntax on config file");
}
