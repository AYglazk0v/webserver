#include "../include/Server.hpp"

namespace {
	void signal_handler(int signal) {
		std::cout << "SIGNAL "<< signal << " from server" << std::endl;
		exit(signal);
	}
}

namespace webserver {

	Server::Server(int argc, char** argv) {
		try {
			signal(SIGPIPE, SIG_IGN);
			signal(SIGINT, signal_handler);
			signal(SIGQUIT, signal_handler);
			signal(SIGTSTP, signal_handler);

			Nginx	nginx(argc, argv);
 //TODO
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
		
	}

} //namespace webserver