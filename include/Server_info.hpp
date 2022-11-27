#ifndef SERVER_INFO_HPP
# define SERVER_INFO_HPP

# include <string>
# include <vector>
# include <map>
# include <set>
# include <iostream>

# include <netinet/ip.h>

# include "Location.hpp"

namespace webserver {
	
	class Server_info {
		
		private:		 
			int									listen_fd_;
			sockaddr_in							addr_;
			int									port_;
			std::string							host_;
			std::vector<std::string>			server_name_;
			std::map<int, std::string>			error_page_;
			std::string							root_;
			std::map<std::string, Location>		location_;

		public:		 
			Server_info() {}
			~Server_info() {}
			Server_info(const Server_info& rhs) { *this = rhs; }
			Server_info& operator=(const Server_info& rhs) {
				listen_fd_ = rhs.listen_fd_;
				addr_ = rhs.addr_;
				port_ = rhs.port_;
				host_ = rhs.host_;
				server_name_ = rhs.server_name_;
				error_page_ = rhs.error_page_;
				root_ = rhs.root_;
				location_ = rhs.location_;
				return *this;
			}

			void init() {
				port_ = -1;
				host_.clear();
				server_name_.clear();
				error_page_.clear();
				root_.clear();
				location_.clear();
			}

			void	setListenFd(const int& repl_listen_fd_)							{ listen_fd_ = repl_listen_fd_; } 
			void	setAddr(sockaddr_in repl_addr)									{ addr_ = repl_addr; }
			void	setPort(const int& repl_potr)									{ port_ = repl_potr; }
			void	setHost(const std::string& repl_host)							{ host_ = repl_host; }
			void	setServerName(const std::vector<std::string>& repl_server_name)	{ server_name_ = repl_server_name; }
			void	setErrorPage(const std::map<int, std::string>& repl_error_page)	{error_page_ = repl_error_page; }
			void	setRoot(const std::string& repl_root)							{ root_ = repl_root; }

			const int&	getListenFd() const 										{ return listen_fd_; }
			const sockaddr_in& getAddr() const 										{ return addr_; }
			const int&	getPort() const 											{ return port_; }
			const std::string& getHost() const 										{ return host_; }
			const std::vector<std::string>& getServerName() const 					{ return server_name_; }
			const std::map<int, std::string>& getErrorPage() const 					{ return error_page_; }
			const std::string& getRoot() const 										{ return root_; } 
			const std::map<std::string, Location>& getLocation() const 				{ return location_; }

			void addLocation(const Location& new_location) {
				location_.insert(std::pair<std::string, Location>(new_location.getPath(), new_location));
			}

			void updateErrorPageParh(const int& err_int, const std::string& new_path) {
				error_page_.find(err_int)->second = new_path;
			}
	}; //Server_info;

}; // server

#endif