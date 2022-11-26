#include "../include/Nginx.hpp"
#include <fstream>

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
					addLocationInServer(server, location); //TODO
				}
				clearSemicolonBuffer(buffer); //TODO
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