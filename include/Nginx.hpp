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
			Nginx(int argcm, char** argv);
		
		private:
			void		nginxPrint() const;
			void		nginxPrintLocation(const Server_info& server) const;

			std::string	findConfigPath(int argc, char** argv);
			void		readConfigFile(const std::string& cfg_path);
			void		clearBufferSpace(std::string &buffer);
			void		checkBuffer(std::string& buffer);
			void		clearSemicolonBuffer(std::string& buffer);
			void		parsingBuffer(const Server_info& new_server, Location& new_location, const std::string& Location);
			
			void		addNewServer(Server_info& new_server);
			void		addLocationInServer(Server_info& server, Location& new_location);
			void		parseListen(Server_info& server, const std::string& buffer_split);
			void		parseListenPort(Server_info& server, const std::string& port);
			void		parseServerName(Server_info& server, const std::vector<std::string>& buffer_split);
			void		parseErrorPage(Server_info& server, const std::string& num_error, const std::string& error_path);
			void		parseRootServ(Server_info& server, const std::string& root);
			void		parseLocationMain(Server_info& server, const std::string& path);
			void		parseLocationAllowMethod(Server_info& server, const std::string& allow_method);
			
			void		parseLocationIndex(Location& location, const std::string& buffer_split);
			void		parseLocationRoot(Location& location, const std::string& root);
			void		parseLocationAutoIndex(Location& location, const std::string& autoindex);
			void		parseUploadPath(Location& location, const std::string& upload_path);
			void		parseUploadEnable(Location& location, const std::string& upload_enable);
			void		parseCgiPath(Location& location, const std::string& cgi_path);
			void		parseCgiExt(Location& location, const std::vector<std::string>& buffer_slpit);
			void		parseReturn(Location& Location, const std::string& ret);
			void		parseLocationClientMaxBidySize(Location& Location, const std::string& client_max_body_size);

			void		checkHostPort(const Server_info& server) const;
			void		checkErrorPagePath(Server_info& server);

	}; //Nginx

} //server

#endif