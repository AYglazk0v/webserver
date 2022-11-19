#ifndef SERVER_INFO_HPP
# define SERVER_INFO_HPP

# include <string>
# include <vector>
# include <map>
# include <set>
# include <netinet/ip.h>

# include "Location.hpp"

namespace server{
	
	class Server_info {
		private:
			int							listen_fd_;
			sockaddr_in					addr_;
			int							port_;
			std::string					host_;
			std::set<std::string>		server_name_;
			std::map<int, std::string>	error_page_;
			std::string 				root_;

			std::vector<Location>		location_;

			
		public:
			Server_info();
			~Server_info();
	}; //Server_info;

}; // server

#endif