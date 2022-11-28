#include "../include/Server.hpp"
#include <fstream>

namespace {
	void signal_handler(int signal) {
		std::cout << "SIGNAL "<< signal << " from server" << std::endl;
		exit(signal);
	}
}

namespace webserver {

	void Server::createHttpCodeList() {
		http_code_list_["200"] = "OK";
		http_code_list_["201"] = "Created";
		http_code_list_["204"] = "No Content";
		http_code_list_["301"] = "Moved Permanently";	
		http_code_list_["400"] = "Bad Request";
		http_code_list_["403"] = "Forbidden";
		http_code_list_["404"] = "Not Found";
		http_code_list_["405"] = "Method Not Allowed";
		http_code_list_["411"] = "Length Required";
		http_code_list_["413"] = "Payload Too Large";
		http_code_list_["414"] = "URI Too Long";
		http_code_list_["500"] = "Internal Server Error";
		http_code_list_["502"] = "Bad Gateway";
		http_code_list_["504"] = "Gateway Timeout";
		http_code_list_["505"] = "HTTP Version Not Supported";
		http_code_list_["508"] = "Loop Detected";
	}


	void Server::clearBuffer(std::string &buffer) {
		int n = buffer.find("#");
		if (n != std::string::npos) {
			buffer = buffer.substr(0, n);
		}
		for (int i = buffer.size() - 1; i >= 0; i--) {
			if (isspace(buffer[i])) {
				buffer[i] = ' ';
			}
			if (isspace(buffer[i]) && (i == 0 || isspace(buffer[i - 1]) || i == buffer.size() - 1)) {
				buffer.erase(i, 1);
			}
		}
		if (buffer.back() != ';') {
			throw std::runtime_error(ERROR_MIME_SEMIKOLON + buffer);
		}
		buffer.pop_back();
	}

	void Server::createMimeExt() {
		std::ifstream mime_read(MIME_PATH);
		if (!mime_read.is_open()) {
			throw std::runtime_error(ERROR_MIME_OPEN + std::string(MIME_PATH));
		}
		std::string buffer;
		while(!mime_read.eof()) {
			std::getline(mime_read, buffer);
			clearBuffer(buffer);
			if(!buffer.empty()) {
				std::vector<std::string> tmp_mime = split(buffer, " ");
				for (size_t i = 1, end =tmp_mime.size(); i < end; ++i) {
					mime_ext_list_.insert(std::pair<std::string, std::string>("." + tmp_mime[1], tmp_mime[0]));
				}
			}
		}
		mime_read.close();
	}

	Server::Server(int argc, char** argv) {
		try {
			signal(SIGPIPE, SIG_IGN);
			signal(SIGINT, signal_handler);
			signal(SIGQUIT, signal_handler);
			signal(SIGTSTP, signal_handler);
			
			Nginx	nginx(argc, argv);
			
			createHttpCodeList();
			createMimeExt();
			
			serverStart(nginx); //TODO
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			exit(EXIT_FAILURE);
		}
	}
	
} //namespace webserver