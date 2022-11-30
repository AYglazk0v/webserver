#ifndef USER_HPP
# define USER_HPP

# include "Server_info.hpp"
# include "utils.hpp"
# include <sys/dir.h>
# include <sys/wait.h>

# define HEADER_FIELD_SIZE		100
# define HEADER_KEY_LENGTH		100
# define HEADER_VALUE_LENGTH	2048

namespace webserver
{
	class User
	{
	private:
		int									user_fd_;
		sockaddr_in							addr_;
		const Server_info*					server_;
		time_t								action_time_;
		std::map<std::string, std::string>*	http_code_list_;
		std::map<std::string, std::string>*	mime_ext_list_;
		
		std::string							request_;
		std::string							request_method_;
		std::string							request_uri_;
		std::string							request_query_;
		std::string							request_protocol_;
		std::map<std::string, std::string>	request_header_;
		std::string							request_body_;

		bool								recv_header_;
		bool								recv_body_;
		bool								is_chunked_;
		bool								is_content_length_;
		bool								content_length_;
		
		const Location*						response_location_;
		std::string							response_;
		int									response_send_pos_;
		std::string							response_path_;
		std::string							response_header_;
		std::string							response_body_;

		bool								response_dir_;
		bool								response_file_;
		std::string							response_ext_;
		std::string							response_header_cookie_;
		
		std::string							status_code_;
		std::string							content_type_cgi_;
		
	private:
		User();

	public:
		User& operator=(const User& rhs);
		User(const User& rhs) { *this = rhs; }
		~User(){}
	
	private:
		bool			recvHeader();
		void			checkBodyLenght();
		bool			recvBody();
		void			parseRequestMethod(const std::string& method_sting);
		void			parseRequestHeader(const std::string& header_string);
		void			recvBodyParseChunked();
		void			recvBodyParseLength();
		void			parseRequestFindLocation();

		void			checkResponsePathUri();
		void			createResponseReturnHeader();
		void			parseResponseGetBody();
		void			parseResponsePostBody();
		void			parseResponseUpload();
		void			parseResponsePutBody();
		void			parseResponseDeleteBody();

		void			createResponseBodyFromFile(const std::string& body_path_to_file);
		void			createResponseErrorBody(const std::string& msg_error);
		void			createResponseBodyDirectory(const std::string& dir_path);

		void			parseResponseCGI();
		void			CGIsetEnv();
		void			CGIparseBody();
	
	public:
		User(const int& user_ft, Server_info* server, const sockaddr_in& addr,
				std::map<std::string, std::string>* http_code,
				std::map<std::string, std::string>* mime_ext_list);
		
		void			clearAll();
		void			requestPrint() const;
		void			responsePrint() const;
		bool			recvRequest(const char* buffer, const size_t& nbytes);
		void			checkAndParseRequest();
		void			createResponse();
		void			createResponseError(const std::string& msg_error);

		const time_t&		getActiveTime() const		{ return action_time_; }
		const std::string&	getRequest() const			{ return request_; }
		const std::string&	getResponse() const			{ return response_; }
		const std::string&	getResponseHeader() const	{ return response_header_; }
		const int			getResponseSendPos() const	{return response_send_pos_; }

		void	UpdateActiveTime() 						{action_time_ = time(0); }
		void	updateResponseSendPos(const int& send)	{response_send_pos_ += send; }
		
	};//User
	
} // namespace webserver

#endif