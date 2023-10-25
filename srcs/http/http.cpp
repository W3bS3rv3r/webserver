#include "http.hpp"
#include "../HTTPException/HTTPException.hpp"
#include "request_utils.hpp"
#include "get.hpp"
#include "post.hpp"
#include "delete.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include <utility>

Response	getResponse(const std::string& request, const Socket& socket) {
	std::stringstream		stream(request);
	std::string				method, route, path;
	Response				response;
	std::string				host = getHeaderValue(request, "Host");
	const VirtualServer&	server = socket.getVServer(host);

	stream >> method;
	stream >> route;
	const Location&	location = server.getLocation(route);
	try {
		response = location.handleRequest(method, route, request, socket);
	}
	catch (HTTPException& e) {
		e.setHost(host);
		throw ;
	}
	return (response);
}
