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
This webserver was tested using the firefox browser during development.

To run the webserver on your machine follow these steps:
1- Clone this repo on your machine:
