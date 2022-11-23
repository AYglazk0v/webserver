#ifndef SERVER_INFO_HPP
# define SERVER_INFO_HPP

# include <string>
# include <vector>
# include <map>
# include <set>

# include <netinet/ip.h>

# include "Location.hpp"

namespace server {
	
	class Server_info {
		
		private:
			int									_listen_fd;
			sockaddr_in							_addr;
			int									_port;
			std::string							_host;
			std::vector<std::string>			_server_name;
			std::map<int, std::string>			_error_page;
			std::string							_root;
			std::map<std::string, Location>		_location;

		public:
			Server_info();
			~Server_info();

	}; //Server_info;

}; // server

#endif