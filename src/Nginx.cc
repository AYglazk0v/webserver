#include "../include/Nginx.hpp"

namespace webserver {

	std::string Nginx::findConfigPath(int argc, char** argv){
		std::string	conf_path;

		if (argc == 1) {
			conf_path = DEFAULT_CONFIG_PATH;
		} else if (argc == 2) {
			conf_path = argv[1];
		} else {
			throw std::runtime_error(ERROR_INPUT_WEBSERV);
		}

		std::string check = DEFAULT_CONFIG_EXTENTION;
		int check_len = check.length();
		if (conf_path.length() <= check_len || conf_path.substr(conf_path.length() - check_len, check_len) != check){
			throw std::runtime_error(ERROR_INPUT_WEBSERV);
		}
		return conf_path;
	}

	void Nginx::clearBufferSpace(std::string& buffer) {
		int n = buffer.find("#");
		if (n != std::string::npos) {
			buffer = buffer.substr(0, n);
		}
		for (int i = buffer.size() - 1; i >= 0; i--) {
			if (isspace(buffer[i])) {
				buffer[i] = ' ';
			}
			if ((isspace(buffer[i]))
				&& (i == 0 || isspace(buffer[i - 1]) || i == buffer.size() - 1)) {
				buffer.erase(i, 1);
			}
		}
	}

	void Nginx::checkBuffer(const std::string& buff) {
		if ((buff == DEFAULT_CONFIG_SERVER && brace_ != 0)
				|| (buff != DEFAULT_CONFIG_SERVER && brace_ == 0)) {
			throw std::runtime_error(ERROR_CONFIG_BRACE_NUMBER);
		}
		for (int i = 0; i < buff.size(); ++i) {
			if (buff[i] == '{') {
				brace_++;
				if (i != buff.size() - 1) {
					throw std::runtime_error(ERROR_CONFIG_SYMBOL_AFTER_OPENNING_BRACE + buff);
				}
				if (!((buff == DEFAULT_CONFIG_SERVER && brace_ == 1)
						|| (buff.substr(0, std::string(DEFAULT_CONFIG_LOCATION).length()) ==  DEFAULT_CONFIG_LOCATION
							&& brace_ == 2))) {
					throw std::runtime_error(ERROR_CONFIG_SYMBOL_WITH_OPENNING_BRACE + buff);
				}
			}
			if (buff[i] == '}') {
				brace_--;
			}
			if (buff[i] == '}' && buff.size() != 1) {
				throw std::runtime_error(ERROR_CONFIG_SYMBOL_WITH_CLOSING_BRACE + buff);
			}
		}
	}

	void Nginx::addLocationInServer(Server_info &server, Location& new_location) {
		if (server.getLocation().find(new_location.getPath()) != server.getLocation().end()) {
			throw std::runtime_error(ERROR_CONFIG_LOCATION_AGAIN);
		}
		if (new_location.getCgiPath().empty() != new_location.getCgiExt().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_CGI_PATH_EXT);
		}
		if (new_location.getAutoindex() == "") {
			new_location.setAutoindex("off");
		}
		if (new_location.getUploadEnable() == "on" && new_location.getUploadPath().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOCATION_UPLOAD_ON_NO_PATH);
		}
		if (new_location.getClientMaxBodySize() == -1) {
			new_location.setClientMaxBodySize(0);
		}
		server.addLocation(new_location);
	}

	void Nginx::clearSemicolonBuffer(std::string& buffer) {
		if (buffer == DEFAULT_CONFIG_SERVER || buffer == "}"
			|| buffer.substr(0, std::string(DEFAULT_CONFIG_LOCATION).length()) == DEFAULT_CONFIG_LOCATION) {
			return;
		}
		if (*(buffer.end() - 1) != ';') {
			throw std::runtime_error(ERROR_CONFIG_NO_SEMIKOLON + buffer);
		}
		buffer.erase(buffer.end() - 1);
		if (isspace(*(buffer.end() - 1))) {
			throw std::runtime_error(ERROR_CONFIG_SEMIKOLON_AFTER_CLEAR + buffer);
		}
	}

	void Nginx::parseListenHost(Server_info& server, const std::string& host) {
		if (host == DEFAULT_CONFIG_LOCAL_HOST) {
			server.setHost("127.0.0.1");
		} else {
			if (inet_addr(host.c_str()) == INADDR_NONE) {
				throw std::runtime_error(ERROR_CONFIG_LISTEN_HOST_WRONG);
			}
			server.setHost(host);
		}
	}

	void Nginx::parseListenPort(Server_info &server, std::string const &port) {
		for (int i = 0; i < port.length(); ++i){
			if (!isdigit(port[i])){
				throw std::runtime_error(ERROR_CONFIG_LISTEN_NOT_DIGIT_PORT + port);
			}
		}
		int tmp_port = std::atoi(port.c_str());
		if (tmp_port < 1024) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_SPECIAL_PORT + port);
		}
		if (tmp_port > 65535) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_TOO_BIG_PORT + port);
		}
		server.setPort(tmp_port);
	}

	void Nginx::parseListen(Server_info& server, const std::string& buffer_split) {
		if (server.getPort() != -1 || !server.getHost().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_AGAIN);
		}
		std::vector<std::string> listen_split = split(buffer_split, ":");
		if (listen_split.size() != 2) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_PARAM + buffer_split);
		}
		parseListenHost(server, listen_split[0]);
		parseListenPort(server, listen_split[1]);
	}

	void Nginx::parseServerName(Server_info& server, const std::vector<std::string>& buffer_split) {
		if (!server.getServerName().empty()) {
			throw std::runtime_error(ERROR_CONFIG_SERVER_NAME_AGAIN);
		}
		std::vector<std::string> server_name;
		for (size_t i = 1, end = buffer_split.size(); i < end; ++i) {
			for (std::vector<std::string>::const_iterator it = server_name.begin(), ite = server_name.end(); it != ite; ++it){
			// if (std::any_of(server_name.begin(), server_name.end(), [&buffer_split, &i](std::string& str){return buffer_split[i] == str;})) {
				if (*it == buffer_split[i]){
					throw std::runtime_error(ERROR_CONFIG_SERVER_NAME_DOUBLE + buffer_split[i]);
				}
			}
			server_name.push_back(buffer_split[i]);
		}
		server.setServerName(server_name);
	}

	void Nginx::parseErrorPage(Server_info& server, const std::string& num_error, const std::string& error_path){
		if (num_error.length() != 3 ) {
			throw std::runtime_error(ERROR_CONFIG_ERROR_PAGE_NOT_THREE_SYM + num_error);
		}
		for (int i = 0; i < num_error.length(); ++i) {
			if (!isdigit(num_error[i])) {
				throw std::runtime_error(ERROR_CONFIG_ERROR_PAGE_NOT_DIGIT_ERROW + num_error);
			}
		}
		int int_error = std::atoi(num_error.c_str());
		if (int_error < 100 || int_error > 599) {
			throw std::runtime_error(ERROR_CONFIG_ERROR_PAGE_WRONG_NUMBER + num_error);
		}
		std::map<int, std::string> error_page = server.getErrorPage();
		if (error_page.find(int_error) != error_page.end()){
			throw std::runtime_error(ERROR_CONFIG_ERROR_PAGE_AGAIN + num_error + " " + error_path);
		}
		error_page.insert(std::pair<int, std::string>(int_error, error_path));
		server.setErrorPage(error_page);
	}
	
	void Nginx::parseRootServ(Server_info& server, std::string& root) {
		if (!server.getRoot().empty()) {
			throw std::runtime_error(ERROR_CONFIG_ROOT_SERV_AGAIN);
		}
		if (!isDirectory(root)) {
			throw std::runtime_error(ERROR_CONFIG_ROOT_DOESNT_EXISTS + root);
		}
		clearDoubleSplash(root);
		server.setRoot(root);
	}
	
	void Nginx::parseLocationMain(Location& location, const std::string& path) {
		location.init();
		if (path[0] != '/') {
			throw std::runtime_error(ERROR_CONFIG_LOCATION_MAIN_PATH);
		}
		location.setPath(path);
	}

	void Nginx::parseLocationAllowMethod(Location& location, std::string& allow_method) {
		if (!location.getAllowMethod().empty()) {
			throw std::runtime_error(ERROR_CONFIG_ALLOW_METHOD_AGAIN);
		}
		if (!(allow_method[0] == '[' && allow_method[allow_method.size() - 1] == ']')
			|| allow_method.length() <= 2) {
			throw std::runtime_error(ERROR_CONFIG_ALLOW_METHOD_FORMAT + allow_method);
		}
		allow_method.erase(allow_method.end() - 1);
		allow_method.erase(0,1);

		std::vector<std::string> allow_method_split = split(allow_method, ",");
		std::set<std::string> set_allow_metod;
		for (size_t i = 0, end = allow_method_split.size(); i < end; ++i) {
			if (location.getAllowMethod().find(allow_method_split[i]) != location.getAllowMethod().end()) {
				throw std::runtime_error(ERROR_CONFIG_ALLOW_METHOD_DOUBLE + allow_method_split[i]);
			}
			if (allow_method_split[i] == DEFAULT_CONFIG_ALLOW_METHOD_GET
				|| allow_method_split[i] == DEFAULT_CONFIG_ALLOW_METHOD_POST
				|| allow_method_split[i] == DEFAULT_CONFIG_ALLOW_METHOD_PUT
				|| allow_method_split[i] == DEFAULT_CONFIG_ALLOW_METHOD_DELETE) {
				set_allow_metod.insert(allow_method_split[i]);
			} else {
				throw std::runtime_error(ERROR_CONFIG_ALLOW_METHOD_UNKNOWN + allow_method_split[i]);
			}
		}
		location.setAllowMethod(set_allow_metod);
	}

	void Nginx::parseLocationIndex(Location& location, const std::string& buffer_split) {
		if (!location.getIndex().empty()) {
			throw std::runtime_error(ERROR_CONFIG_INDEX_AGAIN);
		}
		location.setIndex(buffer_split);
	}

	void Nginx::parseLocationRoot(Location& location, const std::string& root) {
		if (!location.getRoot().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_ROOT_AGAIN);
		}
		location.setRoot(root);
	}

	void Nginx::parseLocationAutoIndex(Location& location, const std::string& autoindex) {
		if (!location.getAutoindex().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_AUTOINDEX_AGAIN);
		}
		if (autoindex != "on" && autoindex != "off") {
			throw std::runtime_error(ERROR_CONFIG_LOC_AUTOINDEX_WRONG +autoindex);
		}
		location.setAutoindex(autoindex);
	}
	
	void Nginx::parseUploadEnable(Location& location, const std::string& upload_enable) {
		if (!location.getUploadEnable().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_UPLOAD_ENABLE_AGAIN);
		}
		if (upload_enable != "on" && upload_enable != "off") {
			throw std::runtime_error(ERROR_CONFIG_LOC_UPLOAD_ENABLE_WRONG);
		}
		location.setUploadEnable(upload_enable);
	}

	void Nginx::parseUploadPath(Location& location, const std::string& upload_path) {
		if (!location.getUploadPath().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_UPLOAD_PATH_AGAIN);
		}
		location.setUploadPath(upload_path);
	}

	void Nginx::parseCgiPath(Location& location, std::string& cgi_path) {
		if (!location.getCgiPath().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_CGI_PATH_AGAIN);
		}
		if (!isFile(cgi_path)) {
			throw std::runtime_error(ERROR_CONFIG_CGI_DOESNT_EXISTS + cgi_path);
		}
		clearDoubleSplash(cgi_path);
		location.setCgiPath(cgi_path);
	}

	void Nginx::parseCgiExt(Location& location, const std::vector<std::string>& buffer_slpit) {
		if (!location.getCgiExt().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_CGI_EXT_AGAIN);
		}
		std::set<std::string> cgi_ext;
		for (size_t i = 1, end = buffer_slpit.size(); i < end; ++i) {
			if (buffer_slpit[i][0] != '.') {
				throw std::runtime_error(ERROR_CONFIG_LOC_CGI_EXT_NO_POINT);
			}
			cgi_ext.insert(buffer_slpit[i]);
		}
		location.setCgiExt(cgi_ext);
	}

	void Nginx::parseReturn(Location& location, const std::string& ret) {
		if (!location.getReturn().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LOC_RETURN_AGAIN);
		}
		location.setReturn(ret);
	}

	void Nginx::parseLocationClientMaxBodySize(Location& location, const std::string& client_max_body_size) {
		if (location.getClientMaxBodySize() != -1) {
			throw std::runtime_error(ERROR_CONFIG_CLIENT_MAX_BODY_AGAIN);
		}
		if (*(client_max_body_size.end() - 1) != 'm'
				&& *(client_max_body_size.end() - 1) != 'k'
				&& *(client_max_body_size.end() - 1) != 'b') {
			throw std::runtime_error(ERROR_CONFIG_CLIENT_MAX_BODY_UNKNOWN_TYPE + client_max_body_size);
		}
		for (size_t i = 0, end = client_max_body_size.length() - 1; i < end; ++i) {
			if (!isdigit(client_max_body_size[i])) {
				throw std::runtime_error(ERROR_CONFIG_CLIENT_MAX_BODY_UNKNOWN_TYPE + client_max_body_size);
			}
		}
		int tmp_cmbs = std::atoi(client_max_body_size.c_str());
		if (*(client_max_body_size.end() - 1) == 'm') {
			location.setClientMaxBodySize(tmp_cmbs * 1024 * 1024);
		} else if (*(client_max_body_size.end() - 1) == 'k') {
			location.setClientMaxBodySize(tmp_cmbs * 1024);
		} else {
			location.setClientMaxBodySize(tmp_cmbs);
		}
	}

	void Nginx::parsingBuffer(Server_info& server, Location& new_location, const std::string& buff) {
		if (buff == DEFAULT_CONFIG_SERVER || buff == "}") {
			return;
		}
		std::vector<std::string> buff_split = split(buff, " ");
		if (brace_ == 1) {
			if (buff_split[0] == DEFAULT_CONFIG_LISTEN && buff_split.size() == 2) {
				parseListen(server, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_SERVER_NAME && buff_split.size() >= 2) {
				parseServerName(server, buff_split);
			} else if (buff_split[0] == DEFAULT_CONFIG_ERROR_PAGE && buff_split.size() == 3) {
				parseErrorPage(server, buff_split[1], buff_split[2]);
			} else if (buff_split[0] == DEFAULT_CONFIG_ROOT && buff_split.size() == 2) {
				parseRootServ(server, buff_split[1]);
			} else {
				throw std::runtime_error(ERROR_CONFIG_PARSING + std::string(" : ") + buff_split[0]);
			}
		} else {
			if (buff_split[0] == DEFAULT_CONFIG_LOCATION && buff_split.size() == 3) {
				parseLocationMain(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_ALLOW_METHOD && buff_split.size() == 2) {
				parseLocationAllowMethod(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_INDEX && buff_split.size() == 2) {
				parseLocationIndex(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_ROOT && buff_split.size() == 2) {
				parseLocationRoot(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_AUTOINDEX && buff_split.size() == 2) {
				parseLocationAutoIndex(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_UPLOAD_ENABLE && buff_split.size() == 2) {
				parseUploadEnable(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_UPLOAD_PATH && buff_split.size() == 2) {
				parseUploadPath(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_CGI_PASS && buff_split.size() == 2) {
				parseCgiPath(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_CGI_EXT && buff_split.size() >= 2) {
				parseCgiExt(new_location, buff_split);
			} else if (buff_split[0] == DEFAULT_CONFIG_RETURN && buff_split.size() == 2) {
				parseReturn(new_location, buff_split[1]);
			} else if (buff_split[0] == DEFAULT_CONFIG_CLIENT_MAX_BODY_SIZE && buff_split.size() == 2) {
				parseLocationClientMaxBodySize(new_location, buff_split[1]);
			} else {
				throw std::runtime_error(ERROR_CONFIG_PARSING + std::string(" : ") + buff_split[0]);
			}
		}
	}

	void Nginx::checkHostPort(const Server_info& server) const {
		if (server.getHost().empty() || server.getPort() == -1) {
			throw std::runtime_error(ERROR_CONFIG_CHECK_NECCESSARY);
		}
		for (std::vector<Server_info>::const_iterator it = server_info_.begin(), ite = server_info_.end(); it != ite; ++it) {
			if (it->getHost() == server.getHost() && it->getPort() == server.getPort()) {
				throw std::runtime_error(ERROR_CONFIG_CHECK_HOST_PORT_AGAIN);
			}
		}
	}
	
	void Nginx::checkErrorPagePath(Server_info& server) {
		for (std::map<int, std::string>::const_iterator it = server.getErrorPage().begin(),
				ite = server.getErrorPage().end(); it != ite; ++it) {
			std::string test_path;
			if (server.getRoot().empty()) {
				test_path = std::string(".") + it->second;
			} else {
				test_path = server.getRoot() + it->second; 
			}
			if (!isFile(test_path)) {
				throw std::runtime_error(ERROR_CONFIG_ERROR_PAGE_DOESNT_EXISTS + test_path);
			}
			clearDoubleSplash(test_path);
			if (it->second != test_path) {
				server.updateErrorPageParh(it->first, test_path);
			}
		}
	}

	void Nginx::addNewServer(Server_info& new_server) {
		checkHostPort(new_server);
		checkErrorPagePath(new_server);
		server_info_.push_back(new_server);
	}

	void Nginx::readConfigFile(const std::string& conf_path) {
		std::ifstream	conf_read(conf_path.c_str());

		if (!conf_read.is_open()) {
			throw std::runtime_error(ERROR_CONFIG_OPEN + conf_path);
		}
		std::string	buffer;
		Server_info	server;
		Location	location;
		while (!conf_read.eof()) {
			std::getline(conf_read, buffer);
			clearBufferSpace(buffer);
			if (!buffer.empty()) {
				if (brace_ == 0) {
					server.init();
					location.init();
				}
				checkBuffer(buffer);
				if (brace_ == 1 && !location.getPath().empty()){
					addLocationInServer(server, location);
				}
				clearSemicolonBuffer(buffer);
				parsingBuffer(server, location, buffer);
				if (brace_ == 0) {
					addNewServer(server);
				}
			}
		}
		if (brace_ != 0) {
			throw std::runtime_error(ERROR_CONFIG_BRACE_NUMBER);
		}
		conf_read.close();
	}

	void Nginx::nginxPrintLocation(const Server_info& server) const {
		for (std::map<std::string, Location>::const_iterator it = server.getLocation().begin(),
				ite = server.getLocation().end(); it != ite; ++it) {
			std::cout << "\tLocation : ";
			std::cout << it->first << std::endl;
			if (!it->second.getAllowMethod().empty()) {
				std::cout << "\t\tallow_method : ";
				for (std::set<std::string>::const_iterator itt = it->second.getAllowMethod().begin(),
						itte = it->second.getAllowMethod().end(); itt != itte; ++itt) {
					std::cout << *itt << " ";
				}
				std::cout << std::endl;
			}
			if (!it->second.getIndex().empty()) {
				std::cout << "\t\tIndex : " << it->second.getIndex() << std::endl;
			}
			if (!it->second.getRoot().empty()) {
				std::cout << "\t\tRoot : " << it->second.getRoot() << std::endl;
			}
			if (it->second.getAutoindex() == "on") {
				std::cout << "\t\tAutoindex : " << it->second.getAutoindex() << std::endl;
			}
			if (it->second.getUploadEnable() == "on") {
				std::cout << "\t\tUpload_enable : " << it->second.getUploadEnable() << std::endl;
				std::cout << "\t\tUpload_path : " << it->second.getUploadPath() << std::endl;
			}
			if(!it->second.getCgiPath().empty()) {
				std::cout << "\t\tcgit_ext : ";
				for (std::set<std::string>::const_iterator itt = it->second.getCgiExt().begin(),
					itte = it->second.getCgiExt().end(); itt != itte; ++itt) {
						std::cout << *itt << " ";
				}
				std::cout << std::endl;
				std::cout << "\t\tcgi_path : " << it->second.getCgiPath() << std::endl;
			}
			if (!it->second.getReturn().empty()) {
				std::cout << "\t\treturn : " << it->second.getReturn() << std::endl;
			}
		}
	}

	void Nginx::nginxPrint() const{
		for (size_t i = 0, end = server_info_.size(); i < end; ++i) {
			std::cout << "___________________________________________________________________________________" << std::endl;
			std::cout << "SERVER " << i + 1 << " info :" << std::endl;
			std::cout << "\tport : " << server_info_[i].getPort()<< std::endl;
			std::cout << "\thost : " << server_info_[i].getHost()<< std::endl;
			if (!server_info_[i].getServerName().empty()) {
				std::cout << "\tserver_name : ";
				for (std::vector<std::string>::const_iterator it = server_info_[i].getServerName().begin(),
						ite = server_info_[i].getServerName().end(); it != ite; ++it) {
					std::cout << *it << " ";			
				}
			}
			if (!server_info_[i].getErrorPage().empty()) {
				std::cout << "\terror_page : " << std::endl;					
				for (std::map<int, std::string>::const_iterator it = server_info_[i].getErrorPage().begin(),
					ite = server_info_[i].getErrorPage().end(); it != ite; ++it) {
						std::cout << "\t\t" << it->first << "\t" << it->second << std::endl;
				}
			}
			if (!server_info_[i].getRoot().empty()) {
				std::cout << "\troot : " << server_info_[i].getRoot() << std::endl;
			}
			if (!server_info_[i].getLocation().empty()) {
				nginxPrintLocation(server_info_[i]);
			}
			std::cout << "___________________________________________________________________________________" << std::endl;
		}
	}

	Nginx::Nginx(int argc, char** argv) {
		std::string	conf_path = findConfigPath(argc, argv);
		brace_ = 0;
		readConfigFile(conf_path);
		#if DEBUG
		nginxPrint();
		#endif
	}

} //namespace webserver