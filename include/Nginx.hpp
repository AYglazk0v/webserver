#ifndef NGINX_HPP
# define NGINX_HPP

# include "Server_info.hpp"

namespace webserver {
	class Nginx {
		private:
			std::vector<Server_info>	server_info_;
			int							brace_;
		
		private:
			Nginx();
			Nginx(const Nginx& rhs);
			Nginx& operator=(const Nginx& rhs);
		public:
			~Nginx() {};
		
		private:
			void		nginxPrint() const;
			void		nginxPrintLocation(const Server_info& server) const;
			std::string	findConfigPath(int argc, char** argv);
			void		readConfigFile(const std::string& cfg_path);
			void		clearBufferSpace(std::string &buffer);
			void		parsingBuffer(const Server_info& new_server, Location& new_location, const std::string& Location);
			void		addNewServer(Server_info& new_server);
			void		addLocationInServer(Server_info& server, Location& new_location);


	}; //Nginx

} //server

#endif