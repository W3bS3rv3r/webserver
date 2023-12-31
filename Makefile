##### SETUP VARIABLES #####

NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

##### C++ VARIABLES #####

MAIN = srcs/main.cpp

CLASSES = $(addprefix srcs/,		\
	Socket/Socket.cpp				\
	Server/Server.cpp				\
	Connection/Connection.cpp		\
	Response/Response.cpp			\
	Request/Request.cpp				\
	Cgi/Cgi.cpp						\
	VirtualServer/VirtualServer.cpp	\
	HTTPException/HTTPException.cpp	\
	Location/Location.cpp			\
)

HTTP = $(addprefix srcs/http/,	\
	http.cpp					\
	get.cpp						\
	post.cpp					\
	delete.cpp					\
	request_utils.cpp			\
)

PARSER = $(addprefix srcs/parser/,	\
		 parser.cpp					\
)

SRCS = $(MAIN) $(CLASSES) $(HTTP) $(PARSER)

OBJS = $(SRCS:.cpp=.o)

##### TEST VARIABLES #####

TESTS = unit-tests/tests-build

##### BUILDING RULES #####

all: $(NAME) default-config

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

CONFIG_FILE = default.conf
DEST_DIR = $(HOME)/w3b53rb3ru5
DEST_FILE = $(DEST_DIR)/w3b53rb3ru5.conf

default-config:
	@mkdir -p $(DEST_DIR)
	@sed 's|~|$(HOME)|g' $(CONFIG_FILE) > $(DEST_FILE)

##### TESTING RULES #####

$(TESTS):
	@cd unit-tests && cmake -S . -B tests-build

test: $(TESTS)
	@cd unit-tests && cmake --build tests-build && cd tests-build && \
		ctest --output-on-failure

cleantest:
	$(RM) $(TESTS)

retest: cleantest test

##### PHONY RULE NAMES #####

.PHONY: re fclean clean all cleantest test
