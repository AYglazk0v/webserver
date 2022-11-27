#include "../include/Nginx.hpp"
#include <fstream>
#include <arpa/inet.h>
#include <algorithm>

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
		if (buffer.back() != ';') {
			throw std::runtime_error(ERROR_CONFIG_NO_SEMIKOLON + buffer);
		}
		buffer.pop_back();
		if (isspace(buffer.back())) {
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

	void Nginx::parseListen(Server_info& server, const std::string& buffer_split) {
		if (server.getPort() != -1 || !server.getHost().empty()) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_AGAIN);
		}
		std::vector<std::string> listen_split = split(buffer_split, ":");
		if (listen_split.size() != 2) {
			throw std::runtime_error(ERROR_CONFIG_LISTEN_PARAM + buffer_split);
		}
		parseListenHost(server, listen_split[0]);
		parseListenHost(server, listen_split[1]);
	}

	void Nginx::parseServerName(Server_info& server, const std::vector<std::string>& buffer_split) {
		if (!server.getServerName().empty()) {
			throw std::runtime_error(ERROR_CONFIG_SERVER_NAME_AGAIN);
		}
		std::vector<std::string> server_name;
		for (size_t i = 1, end = buffer_split.size(); i < end; ++i) {
			for (std::vector<std::string>::const_iterator it = server_name.begin(), ite = server_name.end(); it != ite; ++it){
			// if(std::any_of(server_name.begin(), server_name.end(), [&buffer_split, &i](std::string& str){return buffer_split[i] == str;})) {
				if (*it == buffer_split[i]){
					throw std::runtime_error(ERROR_CONFIG_SERVER_NAME_DOUBLE + buffer_split[i]);
				}
			}
			server_name.push_back(buffer_split[i]);
		}
		server.setServerName(server_name);
	}

	void	parseErrorPage(Server_info& server, const std::string& num_error, const std::string& error_path){
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
	
	void parseLocationMain(Location& location, const std::string& path) {
		location.init();
		if (path[0] != '/') {
			throw std::runtime_error(ERROR_CONFIG_LOCATION_MAIN_PATH);
		}
		location.setPath(path);
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
				parseLocationAllowMethod(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_INDEX && buff_split.size() == 2) {
				parseLocationIndex(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_ROOT && buff_split.size() == 2) {
				parseLocationRoot(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_AUTOINDEX && buff_split.size() == 2) {
				parseLocationAutoIndex(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_UPLOAD_ENABLE && buff_split.size() == 2) {
				parseUploadEnable(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_UPLOAD_PATH && buff_split.size() == 2) {
				parseUploadPath(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_CGI_PASS && buff_split.size() == 2) {
				parseCgiPath(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_CGI_EXT && buff_split.size() >= 2) {
				parseCgiExt(new_location, buff_split); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_RETURN && buff_split.size() == 2) {
				parseReturn(new_location, buff_split[1]); //TODO
			} else if (buff_split[0] == DEFAULT_CONFIG_CLIENT_MAX_BODY_SIZE && buff_split.size() == 2) {
				parseLocationClientMaxBodySize(new_location, buff_split[1]); //TODO
			} else {
				throw std::runtime_error(ERROR_CONFIG_PARSING + std::string(" : ") + buff_split[0]);
			}
		}
	}

	void Nginx::readConfigFile(const std::string& conf_path) {
		std::ifstream	conf_read(conf_path);

		if (!conf_read.is_open()) {
			throw std::runtime_error(ERROR_CONFIG_OPEN + conf_path);
		}
		std::string	buffer;
		Server_info	server;
		Location	location;
		while (conf_read.eof()) {
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
				parsingBuffer(server, location, buffer); //TODO
				if (brace_ == 0) {
					addNewServer(server); //TODO
				}
			}
		}
		if (brace_ != 0) {
			throw std::runtime_error(ERROR_CONFIG_BRACE_NUMBER);
		}
		conf_read.close();
	}

	Nginx::Nginx(int argc, char** argv) {
		std::string	conf_path = findConfigPath(argc, argv);
		brace_ = 0;
		readConfigFile(conf_path); //TODO
		nginxPrint(); //TODO
	}

} //namespace webserver