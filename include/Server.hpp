#ifndef SERVER_HPP
# define SERVER_HPP

# include "Server_info.hpp"
# include "User.hpp"
# include <poll.h>
# include <time.h>

namespace webserver {

	class Server {
		private:
			std::map<int,Server_info>			serv_;
			std::vector<pollfd>					fds_;
			std::map<int, User>					usr_;

			std::map<std::string, std::string>	http_code_list_;
			std::map<std::string, std::string>	mime_ext_list_;
			time_t								check_session_;
			std::set<int>						user_close_;

		public:
			Server() {}
			~Server() {}
	};

} //namespace webserver

#endif