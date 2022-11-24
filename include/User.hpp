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
		
	public:
		User() {}
		~User() {}
	};
	
} // namespace webserver

#endif