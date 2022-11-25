#ifndef SERVER_HPP
# define SERVER_HPP

# include "Server_info.hpp"
# include "Nginx.hpp"
# include "User.hpp"
# include <poll.h>
# include <time.h>

namespace webserver {

	class Server {
		private:
			std::vector<pollfd>					fds_;
			std::map<int,Server_info>			serv_;
			std::map<int, User>					usr_;
			std::map<std::string, std::string>	http_code_list_;
			std::map<std::string, std::string>	mime_ext_list_;

			time_t								check_session_;
			std::set<int>						user_close_;

		private:
			Server();
			Server(const Server& rhs);
			Server& operator=(const Server& rhs);
		
		public:
			~Server();
		 	Server(int argc, char** argv);
			void		Loop();
		
		private:
			void		serverStart(const Nginx& nginx);
			void		socketStart(const int& port, std::string& host, Server_info& tmp_server);
			void		createHttpCodeList();
			void		createMimeExt();
			void		clearBuffer(std::string& buffer);

			void		pollWait();
			void		pollInServer(std::vector<pollfd>::iterator& it);
			void		pollInUser(std::vector<pollfd>::iterator& it);
			void		pollOut(std::vector<pollfd>::iterator& it);
			void		pollElse(std::vector<pollfd>::iterator& it);

			void		closeConnection();
			void		checkUserTimeOut();
	};//server;

} //namespace webserver
#endif