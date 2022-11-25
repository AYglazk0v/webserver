#ifndef USER_HPP
# define USER_HPP

# include "Server_info.hpp"

namespace webserver
{
	class User
	{
	private:
		int									user_fd_;
		sockaddr_in							addr_;
		const Server_info*					server_;
		time_t								action_time_;
		std::map<std::string, std::string>	http_code_list_;
		std::map<std::string, std::string>	mime_ext_list;
		
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
		bool								is_content_lenght_;
		bool								content_lenght_;
		
		const Location*						response_location_;
		std::string							response_;
		int									response_send_pos_;
		std::string							response_path_;
		std::string							response_header;
		std::string							response_body_;

		bool								response_dir_;
		bool								response_file_;
		std::string							response_ext_;
		std::string							response_header_cookie_;
		
		std::string							status_code_;
		std::string							content_type_cgi;
		
	private:
		User();

	public:
		User(const User& rhs);
		User& operator=(const User& rhs);
		~User();
	
	private:
		bool			recvHeader();
		void			checkBodyLenght();
		bool			recvBody();
		void			parseRequestMethod(const std::string& method_sting);
		void			parseRequestHeader(const std::string& header_string);
		void			recvBodyParseChunked();
		void			recvBodyParseLenght();
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
		void			createResponceBodyDirectory(const std::string& dir_path);

		void			parseResponseCGI();
		void			CGIsetEnv();
		void			CGIparseBody();
	
	public:
		User(const int& user_ft, Server_info* server, const sockaddr_in& addr,
				std::map<std::string, std::string>* http_code,
				std::map<std::string, std::string>* mime_ext_list);
		
		void			UpdateActiveTime();
		const time_t&	getActiveTime() const;
		void			clearAll();
		void			requestPrint() const;
		void			responsePrint() const;

		const std::string& getRequest() const;

		const std::string& getResponse() const;
		const std::string& getResponseHeader() const;
		const std::string& getResponseSendPos() const;
	};
	
} // namespace webserver

#endif