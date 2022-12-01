#include "../include/Server.hpp"
#include "../include/utils.hpp"

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
		int ok = 1;
		if ((setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int))) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_SETSOCKOPT);
		}
		if ((fcntl(listen_fd, F_SETFL, O_NONBLOCK)) < 0) {
			close(listen_fd);
			throw std::runtime_error(ERROR_SERVER_FCNTL);
		}
		struct	sockaddr_in serv_addr;
		std::memset(&serv_addr, 0, sizeof(serv_addr));
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
				socketStart(it->getPort(), it->getHost(), tmp_server); 
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

	void Server::pollWait() {
		std::string dot[10] = {"     ", ".    ", "..   ", "...  ", ".... ", ".....", ".... ", "...  ", "..   ", ".    "};
		int poll_count = 0;
		int n = 0;
		while (poll_count == 0) {
			std::cout << SERVER_POOL_WAIT << dot[n++] << std::flush;
			poll_count = poll(&(fds_.front()), fds_.size(), TIMEOUT);
			if (n == 10) {
				checkUserTimeOut();
				n = 0;
			}
			if (poll_count < 0) {
				std::cout << SERVER_POOL_WAIT_MINUS_ONE << std::endl;
				poll_count = 0;
			}
		}
		std::cout << "\r" << currentDataTime() << "\t" << SERVER_POOL_WAIT_CONNECTION << std::endl;
	}

	void Server::pollInServer(std::vector<pollfd>::iterator& it) {
		if (DEBUG == 1) {
			std::cout << SERVER_POOLIN_RECIEVED_SERV << it->fd << std::endl;
		}
		it->revents = 0;
		sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		int user_fd = accept(it->fd, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
		if (user_fd < 0) {
			std::cout << ERROR_SERVER_POOLIN_NEW_CONN << std::endl;
		} else {
			char buff[16];
			inet_ntop(AF_INET, &addr.sin_addr, buff, sizeof(buff));
			std::cout << SERVER_POOLIN_NEW_CONN << user_fd << " from : " << buff << std::endl;
			User new_usr(user_fd, &serv_[it->fd], addr, &http_code_list_, &mime_ext_list_);
			struct  pollfd tmp;
			tmp.fd = user_fd;
			tmp.events = POLLIN;
			tmp.revents = 0;
			if (fcntl(user_fd, F_SETFL, O_NONBLOCK) < 0) {
				std::cout << ERROR_SERVER_POOLIN_FCNTL << std::endl;
				close(user_fd);
				return;
			}
			fds_.push_back(tmp);
			usr_.insert(std::pair<int, User>(user_fd, new_usr));
		}
	}

	void Server::pollInUser(std::vector<pollfd>::iterator& it) {
		if (DEBUG == 1) {
			std::cout << SERVER_POOLIN_RECIEVED_USER << it->fd << std::endl;
		}
		it->revents = 0;
		User *itu = &usr_.find(it->fd)->second;
		itu->UpdateActiveTime();

		char buffer[MAX_BUFFER_RECV];
		int nbytes = recv(it->fd, buffer, MAX_BUFFER_RECV - 1 , 0);
		if (nbytes < 0) {
			std::cout<< ERROR_SERVER_POOLIN_USER_READ << it->fd << std::endl;
			user_close_.insert(it->fd);
		} else if (nbytes == 0) {
			std::cout << SERVER_POOLIN_USER_SESS_END << it->fd << std::endl;
			user_close_.insert(it->fd);
		} else {
			if (itu->recvRequest(buffer, nbytes) == false) {
				return;
			}
			std::cout << SERVER_POOLIN_USER_READ_END << it->fd << std::endl;
			try {
				itu->checkAndParseRequest();
				itu->createResponse();

			} catch (const char* s) {
				std::cout<< s << " " << it->fd << std::endl;
				itu->createResponseError(s);
			} catch (const std::exception& e) {
				std::cerr << e.what() << std::endl;
				itu->createResponseError("500 Unexpected Error...");
			}
			itu->requestPrint();
			itu->responsePrint();
			it->events = POLLOUT;
		}
	}

	void Server::pollOut(std::vector<pollfd>::iterator& it) {
		if (DEBUG == 1) {
			std::cout << SERVER_POOLOUT_RECIEVED << it->fd << std::endl;
		}
		it->revents = 0;
		User *itu = &usr_.find(it->fd)->second;
		itu->UpdateActiveTime();
		int n = 0;
		while(true) {
			int result = send(it->fd, itu->getResponse().c_str() + itu->getResponseSendPos(),
					itu->getResponse().length() - itu->getResponseSendPos(), 0);
			if (result < 0 || (result == 0 && itu->getResponse().length() - itu->getResponseSendPos() > 0)) {
				n++;
			} else {
				itu->updateResponseSendPos(result);
				break ;
			}
			if (n < RETRY_TO_SEND) {
				std::cout<< ERROR_SERVER_POOLIN_USER_SEND << it->fd << std::endl;
				user_close_.insert(it->fd);
				return ;
			}
		}
		if (itu->getResponse().length() - itu->getResponseSendPos() > 0) {
			return ;
		}
		std::cout << SERVER_POOLOUT_USER_SEND_END << it->fd << ". ";
		if (itu->getResponseHeader().find("Connection: close") != std::string::npos) {
			std::cout << SERVER_POOLOUT_CLOSE << std::endl;
			user_close_.insert(it->fd);
		} else {
			std::cout << SERVER_POOLOUT_NOT_CLOSE << std::endl;
			itu->clearAll();
			it->events = POLLIN;
		}
	}

	void Server::pollElse(std::vector<pollfd>::iterator& it) {
		if (DEBUG == 1) {
			std::cout << SERVER_POOLERR_RECIEVED << it->fd << " : ";
		}
		if (it->revents & POLLNVAL) {
			std::cout << SERVER_POLLNVAL << std::endl;
		} else if (it->revents & POLLHUP) {
			std::cout << SERVER_POLLHUP << std::endl;
		} else if (it->revents & POLLERR) {
			std::cout << SERVER_POLLERR << std::endl;
		}
		user_close_.insert(it->fd);
	}

	void Server::closeConnection()
	{
		for (std::set<int>::iterator it = user_close_.begin(), 
				ite = user_close_.end(); it != ite; it++) {
			for (std::vector<pollfd>::iterator itt = fds_.begin(),
					itte = fds_.end(); itt != itte; it++) {
				if (*it == itt->fd) {
					close(itt->fd); 
					usr_.erase(itt->fd);
					fds_.erase(itt);
					break ;
				}
			}
		}
		user_close_.clear();
	}

	void Server::checkUserTimeOut() { //МБ ЕСТЬ ОШИБКА ЧЕНКУТЬ!!!
		if (time(0) - check_session_ < TIMEOUT_USER / 2) {
			return ;
		}
		for (std::map<int, User>::iterator it = usr_.begin(), ite = usr_.end(), itt; it !=ite; itt = it++) {
			if ((time(0) - it->second.getActiveTime()) >= TIMEOUT_USER) {
				int del_fd = itt->first;
				for (std::vector<pollfd>::iterator it_fd = fds_.begin(),
						ite_fd = fds_.end(); it_fd != ite_fd; ++it_fd) {
					if (it_fd->fd == del_fd) {
						if (itt->second.getRequest().empty()) {
							close(it_fd->fd);
							fds_.erase(it_fd);
							usr_.erase(itt);
							std::cout << "\t\ttimeout: user disconected " << del_fd << std::endl;
						} else {
							it_fd->events = POLLOUT;
							itt->second.createResponseError("504 ... send to close TIMEOUT");
						}
						break ;
					}
				}
			}
		}
		check_session_ = time(0);
	}

	void Server::Loop() {
		while (true) {
			if (DEBUG == 1) {
				std::cout << "Numbeer of listenning fd : " << fds_.size() << std::endl;
			}
			pollWait();
			for (std::vector<pollfd>::iterator it = fds_.begin(), ite = fds_.end(); it != ite; ++it) {
				if (it->revents == 0) {
					continue;
				}
				if (it->revents & POLLIN && serv_.find(it->fd) != serv_.end()) {
					pollInServer(it);
					break;
				} else if (it->revents & POLLIN) {
					pollInUser(it);
				} else if (it->revents & POLLOUT) {
					pollOut(it);
				} else {
					pollElse(it);
				}
			}
			closeConnection();
			checkUserTimeOut();
		}
	}

} //namespace webserver