#ifndef CGI_HPP
# define CGI_HPP

class CGI {
public:
	CGI(void);
	~CGI(void);

private:
	CGI(const CGI& src);
	CGI&	operator=(const CGI& src);

};

#endif