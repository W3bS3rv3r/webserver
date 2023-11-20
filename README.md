# webserver
This project was developed by [supermeia](https://www.linkedin.com/in/gabrielclcardoso/) and [Lucas Fads](https://www.linkedin.com/in/lucasfads/)
## Specs
This project aims to repdroduce some of [nginx](https://nginx.org/en/)'s behavior.

The webserver implements the following functionalities:
* Serves a fully static website;
* Has GET, POST and DELETE methods;
* Interfaces with Python cgi for POST and GET methods.

## Config
Through the config file you can set the following functionalities:
* Chose the port of a server;
* Setup the server_name;
* Limit client body size;
* Setup routes (no regexp);
  * Define accepted methods for that route;
  * Define HTTP redirection;
  * Define the root directory (where files will be served from);
  * Turn directory listing on/off
  * Set an index file;
  * Execute cgi based on file extension (.py for example);
  * Configure where uploaded files will be stored.

## Testing the webserver
This webserver was tested using the firefox browser during development and it only runs on linux.

To run the webserver on your machine follow these steps:
1) Make sure you have `python` and `make` installed:
```bash
make -v; python3 --version
```
If the command above doesn't return any error messages you're good to go!

2) Clone this repo on your machine:
```bash
git clone https://github.com/W3bS3rv3r/webserver
```
3) Enter the repository and compile the program
```bash
cd webserver && make
```
4) change the paths of the config files to match the path of your machine:
```bash
sed -i 's|demo_path|'"$(pwd)"'|g' www_demo.conf &&
sed -i "s|home|$HOME|g" default.conf
```
5) Run the server with the demo config and then open http://localhost:4343/ on your browser to see it in action:
```bash
./webserv www_demo.conf
```
