#include "parser.hpp"
#include <map>
#include <string>
#include <iostream>
#include <sstream>

Socket* newSocket(std::fstream& file) {
	Socket*								socket;
	std::map<std::string, std::string>	parameters = readParameters(file);

	try {
		socket = new Socket(parameters);
	}
	catch (const std::exception& e) {
		std::string	port;

		if (parameters.find("port") != parameters.end())
			port = parameters["port"];
		else
			port = "80";
		delete socket;
		socket = NULL;
		std::cerr << "failed to create socket on port: " << port << std::endl;
		std::cerr << "Error: " << e.what() << std::endl; 
	}
	return (socket);
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
	if (field == "port" || field == "location" || field == "cgi_extension")
		return true;
	return false;
}

const char*	InvalidSyntaxException::what(void) const throw() {
	return ("Invalid syntax on config file");
}
