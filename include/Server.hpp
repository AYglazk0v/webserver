#ifndef SERVER_HPP
# define SERVER_HPP

# include "Server_info.hpp"
# include "Nginx.hpp"
# include "User.hpp"
# include <poll.h>
# include <signal.h>
# include <time.h>


# define MIME_PATH			"./config/mime.conf"
# define MAX_BUFFER_RECV	65535	// max possible read
# define MAX_LISTEN			1024	// number of listen
# define TIMEOUT 			100		// milisecond
# define TIMEOUT_USER 		30		// second
# define RETRY_TO_SEND 		5		// try to send times


# define ERROR_MIME_OPEN					"MIME ERROR: No file in : "
# define ERROR_MIME_SEMIKOLON				"MIME ERROR: No semikolon at the end : "

# define ERROR_SERVER_SOCKET				"SERVER ERROR: socket() failed"
# define ERROR_SERVER_SETSOCKOPT			"SERVER ERROR: setsockopt() failed"
# define ERROR_SERVER_FCNTL					"SERVER ERROR: fcntl() failed"
# define ERROR_SERVER_BIND					"SERVER ERROR: binding connection. Socket has already been establishng"
# define ERROR_SERVER_LISTEN				"SERVER ERROR: Listen on web port through socket file descriptor failed"
# define ERROR_SERVER_POLL					"SERVER ERROR: POLL error"
# define ERROR_SERVER_NOSERVER				"SERVER ERROR: NO SERVER"
# define SERVER_START						"!!!SERVER START!!!"
# define SERVER_WARNING						"!!!SERVER DONT START!!!"

# define SERVER_POOL_WAIT					"\rWaiting connection"
# define SERVER_POOL_WAIT_MINUS_ONE			"\rPOLL ERROR: poll = -1  "
# define SERVER_POOL_WAIT_CONNECTION		"Recieved connection"
# define SERVER_POOLIN_RECIEVED_SERV		"_PollInServ : "
# define SERVER_POOLIN_RECIEVED_USER		"_PollInUser : "
# define SERVER_POOLOUT_RECIEVED			"_PollOut"
# define SERVER_POOLERR_RECIEVED			"_PollElse : "

# define ERROR_SERVER_POOLIN_NEW_CONN		"\tACCEPT ERROR: could not accept new connection"
# define ERROR_SERVER_POOLIN_FCNTL			"\tFCNTL ERROR: could not do fcntl()"
# define SERVER_POOLIN_NEW_CONN				"\tNew incoming connection on fd : "

# define ERROR_SERVER_POOLIN_USER_READ		"\tError read from : "
# define SERVER_POOLIN_USER_SESS_END		"\tClient ended session from : "
# define SERVER_POOLIN_USER_READ_END		"\tOK Reading ended from : "

# define ERROR_SERVER_POOLIN_USER_SEND		"\tError send from : "
# define SERVER_POOLOUT_USER_SEND_END		"\tOK Sending ended from : "
# define SERVER_POOLOUT_CLOSE				"\tConnection CLOSE."
# define SERVER_POOLOUT_NOT_CLOSE			"\tConnection NOT close."

# define SERVER_POLLNVAL					"Socket was not created"
# define SERVER_POLLHUP						"Socket broken connection"
# define SERVER_POLLERR						"Error occurred with socket"

namespace webserver {

	class Server {
		private:
			std::vector<pollfd>					fds_;
			std::map<int,Server_info>			serv_;
			// std::map<int, User>					usr_;
			std::map<std::string, std::string>	http_code_list_;
			std::map<std::string, std::string>	mime_ext_list_;

			time_t								check_session_;
			std::set<int>						user_close_;

		private:
			Server();
			Server(const Server& rhs);
			Server& operator=(const Server& rhs);
		
		public:
			~Server() {};
		 	Server(int argc, char** argv);
			void		Loop();
		
		private:
			void		serverStart(const Nginx& nginx);
			void		socketStart(const int& port, const std::string& host, Server_info& tmp_server);
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