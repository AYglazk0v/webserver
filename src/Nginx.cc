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

	Nginx::Nginx(int argc, char** argv) {
		std::string	conf_path = findConfigPath(argc, argv);
		brace_ = 0;
		readConfigFile(conf_path); //TODO
		nginxPrint(); //TODO
	}

} //namespace webserver