#include "../include/Server.hpp"

int	main(int argc, char** argv) {
	webserver::Server srv(argc, argv);
	srv.Loop(); 
	return (0);
}