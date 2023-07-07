##### SETUP VARIABLES #####

NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

##### C++ VARIABLES #####

MAIN = srcs/main.cpp

CLASSES = srcs/Socket/Socket.cpp

HTTP = $(addprefix srcs/http/,	\
	http.cpp	\
	get.cpp		\
)

SRCS = $(MAIN) $(CLASSES) $(HTTP)

OBJS = $(SRCS:.cpp=.o)

##### TEST VARIABLES #####

TESTS = unit-tests/tests-build

##### BUILDING RULES #####

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
