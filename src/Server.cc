#include "../include/Server.hpp"
#include <fstream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>

namespace {
	void signal_handler(int signal) {
		std::cout << "SIGNAL "<< signal << " from server" << std::endl;
		exit(signal);
	}
	
	const std::string currentDataTime() {
		time_t now = time(0);
		struct tm tstruct;
		char buff [80];
		tstruct = *localtime(&now);
		strftime(buff, sizeof(buff), "[ %Y-%m-%d %X ]  :  ", &tstruct);
		return buff;
	} 
} //namespace

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
	
	void Server::socketStart(const int& port, const std::string& host, Server_info& tmp_serv) {
		int listen_fd;
		listen_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (listen_fd < 0) {
			throw std::runtime_error(ERROR_SERVER_SOCKET);
		}
		if ((setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_SETSOCKOPT);
		}
		if ((fcntl(listen_fd, F_SETFL, O_NONBLOCK)) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_FCNTL);
		}
		struct	sockaddr_in serv_addr;
		std::fill(&serv_addr,&serv_addr + sizeof(serv_addr),0);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(host.c_str());
		serv_addr.sin_port = htons(port);
		if ((bind(listen_fd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr))) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_BIND);
		}
		if ((listen(listen_fd, MAX_LISTEN)) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_LISTEN);
		}
		tmp_serv.setListenFd(listen_fd);
		tmp_serv.setAddr(serv_addr);
	}

	void Server::serverStart(const Nginx& nginx) {
		std::cout << "___________________________________________________________________________________" << std::endl;
		for (std::vector<Server_info>::const_iterator it = nginx.getServer().begin(),
			ite = nginx.getServer().end(); it != ite; ++it) {
			try {
				Server_info tmp_server = *it;
				socketStart(it->getPort(), it->getHost(), tmp_server); //TODO
				struct  pollfd tmp;
				tmp.fd = tmp_server.getListenFd();
				tmp.events = POLLIN;
				tmp.revents = 0;
				fds_.push_back(tmp);
				serv_.insert(std::pair<int, Server_info>(tmp_server.getListenFd(), tmp_server));
				std::cout << currentDataTime() << SERVER_START << " on " <<
						 it->getHost() << ":" << it->getPort() << std::endl;
			} catch(const std::exception& e) {
				std::cerr << currentDataTime() << SERVER_WARNING << " on " <<
						it->getHost() << ":" << it->getPort() << std::endl;
				std::cerr << "\t" << e.what() << '\n';
			}
		}
		std::cout << "___________________________________________________________________________________" << std::endl;
		if (serv_.size() == 0) {
			throw std::runtime_error(ERROR_SERVER_NOSERVER);
		}
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
			
			serverStart(nginx);
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			exit(EXIT_FAILURE);
		}
	}

	void Server::Loop() {
		while (true) {
			if (DEBUG == 1) {
				std::cout << "Numbeer of listenning fd : " << fds_.size() << std::endl;
			}
			pollWait(); //TODO
			for (std::vector<pollfd>::iterator it = fds_.begin(), ite = fds_.end(); it != ite; ++it) {
				if (it->revents == 0) {
					continue;
				}
				if (it->revents & POLLIN && serv_.find(it->fd) != serv_.end()) {
					pollInServer(it); //TODO
					break;
				} else if (it->revents & POLLIN) {
					pollInUser(it); //TODO
				} else if (it->revents & POLLOUT) {
					pollOut(it); //TODO
				} else {
					pollElse(it); //TODO
				}
			}
			closeConnection(); //TODO
			checkUserTimeOut(); //TODO
		}
	}

} //namespace webserver