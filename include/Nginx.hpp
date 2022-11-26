#ifndef NGINX_HPP
# define NGINX_HPP

# include "Server_info.hpp"

# define DEFAULT_CONFIG_PATH					"./config/default.conf"
# define DEFAULT_CONFIG_EXTENTION				".conf"
# define DEFAULT_CONFIG_SERVER					"server {"
# define DEFAULT_CONFIG_LISTEN					"listen"
# define DEFAULT_CONFIG_SERVER_NAME				"server_name"
# define DEFAULT_CONFIG_ERROR_PAGE				"error_page"
# define DEFAULT_CONFIG_CLIENT_MAX_BODY_SIZE	"client_max_body_size"
# define DEFAULT_CONFIG_ROOT					"root"
# define DEFAULT_CONFIG_LOCATION				"location"
# define DEFAULT_CONFIG_ALLOW_METHOD			"allow_methods"
# define DEFAULT_CONFIG_ALLOW_METHOD_GET		"GET"
# define DEFAULT_CONFIG_ALLOW_METHOD_POST		"POST"
# define DEFAULT_CONFIG_ALLOW_METHOD_PUT		"PUT"
# define DEFAULT_CONFIG_ALLOW_METHOD_DELETE		"DELETE"
# define DEFAULT_CONFIG_INDEX					"index"
# define DEFAULT_CONFIG_AUTOINDEX				"autoindex"
# define DEFAULT_CONFIG_UPLOAD_ENABLE			"upload_enable"
# define DEFAULT_CONFIG_UPLOAD_PATH				"upload_path"
# define DEFAULT_CONFIG_CGI_PASS				"cgi_path"
# define DEFAULT_CONFIG_CGI_EXT					"cgi_ext"
# define DEFAULT_CONFIG_RETURN					"return"

# define ERROR_INPUT_WEBSERV						"INPUR ERROR! Too many arguments! Usage : ./webserv [ config_file.conf ]"
# define ERROR_CONFIG_EXTENSION						"CONFIG ERROR! Wrong config extension! Usage : [ *.conf ]"
# define ERROR_CONFIG_OPEN							"CONFIG ERROR! Cannot open config file : "
# define ERROR_CONFIG_BRACE_NUMBER					"CONFIG ERROR! Wrong number of brace during parser"
# define ERROR_CONFIG_SYMBOL_AFTER_OPENNING_BRACE	"CONFIG ERROR! Wrong symbol after brace { : "
# define ERROR_CONFIG_SYMBOL_WITH_OPENNING_BRACE	"CONFIG ERROR! Wrong symbol with brace { : "
# define ERROR_CONFIG_SYMBOL_WITH_CLOSING_BRACE		"CONFIG ERROR! Any symbol with brace } : "
# define ERROR_CONFIG_NO_SEMIKOLON					"CONFIG ERROR! Wrong semikolon : "
# define ERROR_CONFIG_SEMIKOLON_AFTER_CLEAR			"CONFIG ERROR! Wrong line after del semikolon : "

# define ERROR_CONFIG_PARSING						"CONFIG ERROR! Unknown parameter format/type"

# define ERROR_CONFIG_LISTEN_AGAIN					"CONFIG ERROR! Listen again"
# define ERROR_CONFIG_LISTEN_PARAM					"CONFIG ERROR! Wrong listen parameter [ host:port ]. But : "
# define ERROR_CONFIG_LISTEN_HOST_WRONG				"CONFIG ERROR! Wrong format host : "
# define ERROR_CONFIG_LISTEN_NOT_DIGIT_PORT			"CONFIG ERROR! Port is not digit : "
# define ERROR_CONFIG_LISTEN_SPECIAL_PORT			"CONFIG ERROR! Port incorrect (special port 0 - 1024) : "
# define ERROR_CONFIG_LISTEN_TOO_BIG_PORT			"CONFIG ERROR! Port incorrect (too big, max 65535) : "
# define ERROR_CONFIG_SERVER_NAME_AGAIN				"CONFIG ERROR! Server_name again"
# define ERROR_CONFIG_SERVER_NAME_DOUBLE			"CONFIG ERROR! Server_name doubel : "
# define ERROR_CONFIG_ERROR_PAGE_AGAIN				"CONFIG ERROR! Error_page again : "
# define ERROR_CONFIG_ERROR_PAGE_NOT_THREE_SYM		"CONFIG ERROR! Error_page is too big : "
# define ERROR_CONFIG_ERROR_PAGE_NOT_DIGIT_ERROW	"CONFIG ERROR! Error_page is not digit : "
# define ERROR_CONFIG_ERROR_PAGE_WRONG_NUMBER		"CONFIG ERROR! Error_page is wrong int [100...600] : "
# define ERROR_CONFIG_ROOT_SERV_AGAIN				"CONFIG ERROR! Root in serev again"
# define ERROR_CONFIG_ROOT_DOESNT_EXISTS			"CONFIG ERROR! Root in serev doesnt exists : "

# define ERROR_CONFIG_LOCATION_MAIN_PATH			"CONFIG ERROR! Location format wrong [ /.... ]"
# define ERROR_CONFIG_ALLOW_METHOD_AGAIN			"CONFIG ERROR! Allow_method again"
# define ERROR_CONFIG_ALLOW_METHOD_FORMAT			"CONFIG ERROR! Allow_method format [...] : "
# define ERROR_CONFIG_ALLOW_METHOD_DOUBLE			"CONFIG ERROR! Double method : "
# define ERROR_CONFIG_ALLOW_METHOD_UNKNOWN			"CONFIG ERROR! Allow_method format unknown : "
# define ERROR_CONFIG_INDEX_AGAIN					"CONFIG ERROR! Index again"
# define ERROR_CONFIG_LOC_ROOT_AGAIN				"CONFIG ERROR! Root in location again"
# define ERROR_CONFIG_LOC_AUTOINDEX_AGAIN			"CONFIG ERROR! Autoindex in location again"
# define ERROR_CONFIG_LOC_AUTOINDEX_WRONG			"CONFIG ERROR! Autoindex in location wrong parameter [on/off] : "
# define ERROR_CONFIG_LOC_UPLOAD_ENABLE_AGAIN		"CONFIG ERROR! Upload_enable in location again"
# define ERROR_CONFIG_LOC_UPLOAD_ENABLE_WRONG		"CONFIG ERROR! Upload_enable in location wrong parameter [on/off] : "
# define ERROR_CONFIG_LOC_UPLOAD_PATH_AGAIN			"CONFIG ERROR! Upload_path in location again"
# define ERROR_CONFIG_LOC_CGI_PATH_AGAIN			"CONFIG ERROR! Cgi_path in location again"
# define ERROR_CONFIG_CGI_DOESNT_EXISTS				"CONFIG ERROR! Cgi_path doesn't exists : "
# define ERROR_CONFIG_LOC_CGI_EXT_AGAIN				"CONFIG ERROR! Cgi_ext in location again"
# define ERROR_CONFIG_LOC_CGI_EXT_NO_POINT			"CONFIG ERROR! Cgi_ext no point in it : "
# define ERROR_CONFIG_LOC_RETURN_AGAIN				"CONFIG ERROR! Return in location again"
# define ERROR_CONFIG_CLIENT_MAX_BODY_AGAIN			"CONFIG ERROR! Client_max_body_size again"
# define ERROR_CONFIG_CLIENT_MAX_BODY_UNKNOWN_TYPE	"CONFIG ERROR! Client_max_body_size must be 'm' or 'k' or 'b': "
# define ERROR_CONFIG_CLIENT_MAX_BODY_IS_NOT_DIGIT	"CONFIG ERROR! Client_max_body_size must be digit : "

# define ERROR_CONFIG_LOCATION_AGAIN				"CONFIG ERROR! Location again"
# define ERROR_CONFIG_LOC_CGI_PATH_EXT				"CONFIG ERROR! Cgi_ext || Cgi_path is wrong"
# define ERROR_CONFIG_LOCATION_UPLOAD_ON_NO_PATH	"CONFIG ERROR! Upload ON, but there is no path"
# define ERROR_CONFIG_CHECK_NECCESSARY				"CONFIG ERROR! No neccessary parameters host:port"
# define ERROR_CONFIG_CHECK_HOST_PORT_AGAIN			"CONFIG ERROR! Host:Port again"
# define ERROR_CONFIG_ERROR_PAGE_DOESNT_EXISTS		"CONFIG ERROR! Error_page is wrong. The file doesnt exist : "
# define ERROR_CONFIG_LOCATION_ROOT_DOESNT_EXISTS	"CONFIG ERROR! Loc Root is wrong. The root doesnt exist : "
# define ERROR_CONFIG_LOC_UPLOAD_PATH_DOESNT_EXISTS	"CONFIG ERROR! Loc Upload_Path is wrong. The upload_path doesnt exist : "
# define ERROR_CONFIG_LOC_INDEX_PATH_DOESNT_EXISTS	"CONFIG ERROR! Loc Index is wrong. The index doesnt exist : "

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
			Nginx(int argc, char** argv);
		
		private:
			void		nginxPrint() const;
			void		nginxPrintLocation(const Server_info& server) const;

			std::string	findConfigPath(int argc, char** argv);
			void		readConfigFile(const std::string& cfg_path);
			void		clearBufferSpace(std::string &buffer);
			void		checkBuffer(const std::string& buffer);
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