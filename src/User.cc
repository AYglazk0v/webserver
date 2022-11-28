#include "../include/User.hpp"
#include "../include/utils.hpp"

namespace webserver {

	User::User(const int& user_fd, Server_info *server, const sockaddr_in& addr,
				std::map<std::string, std::string> *http_code,
				std::map<std::string, std::string> *mime_ext_list) {
		user_fd_ = user_fd;
		addr_ = addr;
		server_ = server;
		action_time_ = time(0);
		http_code_list_ = http_code;
		mime_ext_list_ = mime_ext_list;
		recv_header_ = false;
		recv_body_ = false;
		is_chunked_ = false;
		is_content_length_ = false;
		content_length_ = 0;
		response_location_ = NULL;
		response_send_pos_ = 0;
		response_dir_ = false;
		response_file_ = false;
	}

	User &User::operator=(const User& rhs) {
		user_fd_ = rhs.user_fd_;
		addr_ = rhs.addr_;
		server_ = rhs.server_;
		action_time_ = rhs.action_time_;
		http_code_list_ = rhs.http_code_list_;
		mime_ext_list_ = rhs.mime_ext_list_;
		request_ = rhs.request_;
		request_method_ = rhs.request_method_;
		request_uri_ = rhs.request_uri_;
		request_query_ = rhs.request_query_;
		request_protocol_ = rhs.request_protocol_;
		request_header_ = rhs.request_header_;
		request_body_ = rhs. request_body_;
		is_chunked_ = rhs.is_chunked_;
		is_content_length_ = rhs.is_content_length_;
		content_length_ = rhs.content_length_;
		response_location_ = rhs.response_location_;
		response_ = rhs.response_;
		response_send_pos_ = rhs.response_send_pos_;
		response_path_ = rhs.response_path_;
		response_header_ = rhs.response_header_;
		response_body_ = rhs.response_body_;
		response_dir_ = rhs.response_dir_;
		response_file_ = rhs.response_file_;
		response_ext_ = rhs.response_ext_;
		response_header_cookie_ = rhs.response_header_cookie_;
		status_code_ = rhs.status_code_;
		content_type_cgi_ = rhs.content_type_cgi_;
		return *this;
	}

	void User::clearAll() {
		request_.clear();
		request_method_.clear();
		request_uri_.clear();
		request_protocol_.clear();
		request_header_.clear();
		request_body_.clear();
		recv_header_ = false;
		recv_body_ = false;
		is_chunked_ = false;
		is_content_length_ = false;
		content_length_ = 0;
		response_location_ = NULL;
		response_.clear();
		response_send_pos_ = 0;
		response_path_.clear();
		response_header_.clear();
		response_body_.clear();
		response_dir_ = false;
		response_file_ = false;
		response_ext_.clear();
		response_header_cookie_.clear();
		status_code_.clear();
		content_type_cgi_.clear();
	}

	void User::requestPrint() const {
		std::cout << "___________________________________________________________________________________" << std::endl;
		std::cout << "REQUEST from " << user_fd_ << " with lenght : " << request_.length() << std::endl;
		std::cout << "\tMethod : " << request_method_ << std::endl;
		std::cout << "\tUri : " << request_uri_ << std::endl;
		std::cout << "\tQuery : " << request_query_ << std::endl;
		std::cout << "\tProtocol : " << request_protocol_ << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = request_header_.begin(),
				ite = request_header_.end(); it != ite; ++it) {
			std::cout << "\t" << it->first << " : " << it->second << std::endl;
		}
		std::cout << "\tBody : " << request_body_.length() << std::endl;
		if (request_body_.length() > 1000) {
			std::cout << request_body_.substr(0, 1000) << std::endl;
		} else {
			std::cout << request_body_ << std::endl;
		}
		std::cout << "___________________________________________________________________________________" << std::endl;
	}

	void User::responsePrint() const {
		std::cout << "___________________________________________________________________________________" << std::endl;
		std::cout << "Response from " << user_fd_ << " with lenght : " << response_.length() << std::endl;
		std::cout << response_header_;
		std::cout << "BODY : " << response_body_.length() << std::endl;
		if (response_body_.length() > 1000) {
			std::cout << response_body_.substr(0, 1000) << std::endl;
		} else {
			std::cout << response_body_ << std::endl;
		}
		std::cout << "___________________________________________________________________________________" << std::endl;
	}

	void User::checkBodyLenght() {
		if (request_.find("Transfer-Encoding: chunked", 0) != std::string::npos) {
			is_chunked_ = true;
		} else if (request_.find("Content-Length: ", 0) != std::string::npos) {
			is_content_length_ = true;
			std::string len_type = "Content-Length: ";
			size_t len_start = request_.find(len_type) + len_type.length();
			size_t len_end = request_.find("\r\n", len_start);
			content_length_ = std::atoi(request_.substr(len_start, len_end - len_start).c_str());
		}
	}

	bool User::recvHeader() {
		if (request_.find("\r\n\r\n", 0) != std::string::npos) {
			recv_header_ = true;
			checkBodyLenght();
			return true;
		}
		return false;
	}

	bool User::recvBody() {
		if (is_chunked_ && request_.find("0\r\n\r\n", 0) == std::string::npos) {
			return false;
		} else if (is_content_length_ && request_.size() < request_.find("\r\n\r\n", 0) + 4 + content_length_) {
			return false;
		}
		recv_body_ = true;
		return true;
	}

	bool User::recvRequest(const char* buffer, const size_t& nbytes) {
		request_.append(buffer, nbytes);
		if (recv_header_ == false && recvHeader() == false) {
			return false;
		}
		if (recv_body_ == false && recvBody() == false) {
			return false;
		}
		return true;
	}

	void User::parseRequestMethod(const std::string& method_string) {
		std::vector<std::string> msg = split(method_string, " ");
		if (msg.size() != 3) {
			request_protocol_ = "HTTP/1.1";
			throw "400 REQUEST ERROR! Wrong first line in reqest";
		}
		request_method_ = msg[0];
		int find_path_delim = msg[1].find("?");
		if (find_path_delim == std::string::npos) {
			request_uri_ = msg[1];
			request_query_ = "";		
		} else {
			request_uri_ = msg[1].substr(0, find_path_delim);
			request_query_ = msg[1].substr(find_path_delim + 1);
		}
		request_protocol_ = msg[2];
		if (msg[0] != "GET" && msg[0] != "POST" && msg[0] != "PUT" && msg[0] != "DELETE") {
			throw "405 REQUEST ERROR! Wrong method in reqest";
		}
		if (msg[1].length() > 2048) {
			throw "414 REQEST ERROR! Wrong URI length";
		}
		if (msg[2] != "HTTP/1.1" && msg[2] != "HTTP/1.0") {
			throw "505 REQUEST ERROR! Wrong protocol in reqest";
		}
	}

	void User::parseRequestHeader(const std::string& header_string)
	{
		std::string delimeter= ": ";
		int find_delim = header_string.find(delimeter, 0);
		if (find_delim == std::string::npos) {
			throw "400 REQUEST ERROR! Header mistake, no delimeter ':'";
		}
		int value_start = find_delim + delimeter.length();
		std::string key = header_string.substr(0, find_delim);
		if (key.size() > HEADER_KEY_LENGTH) {
			throw "400 REQUEST ERROR! Header key too big";
		}
		std::string value = header_string.substr(value_start, header_string.size() - value_start);
		if (value.size() > HEADER_VALUE_LENGTH) {
			throw "400 REQUEST ERROR! Header value too big";
		}
		request_header_.insert(std::pair<std::string, std::string>(key, value));
		if (request_header_.size() > HEADER_FIELD_SIZE) {
			throw "400 REQUEST ERROR! Header fields too much";
		}
	}

	void User::recvBodyParseChunked() {
		std::string new_row = "\r\n";
		int len_new_row = new_row.length();
		int pars_position = request_.find("\r\n\r\n") + 4;
		int header_len = request_.find("\r\n0\r\n\r\n");

		while (pars_position < header_len) {
			int find_pos = request_.find(new_row, pars_position);
			int msg_len= std::atoi(request_.substr(pars_position, find_pos - pars_position).c_str());
			pars_position = find_pos + len_new_row;
			find_pos = request_.find(new_row, pars_position);
			std::string msg  = request_.substr(pars_position, find_pos - pars_position).c_str();
			request_body_ += msg;
			pars_position = find_pos + len_new_row;
		}
		content_length_ = request_body_.size();
	}

	void User::checkAndParseRequest() {
		std::string new_row = "\r\n";
		int len_new_row = new_row.length();
		int pars_position = 0;
		int header_len = request_.find("\r\n\r\n");

		while (pars_position < header_len) {
			int find_pos = request_.find(new_row, pars_position);
			std::string msg = request_.substr(pars_position, find_pos - pars_position);
			if (request_method_.empty()) {
				parseRequestMethod(msg);
			} else if (!msg.empty()) {
				parseRequestHeader(msg);
			}
			pars_position = find_pos + len_new_row;
		}
		if (is_chunked_) {
			recvBodyParseChunked();
		} else if  (is_content_length_) {
			recvBodyParseLength();
		}

		parseRequestFindLocation();
		if (response_location_->getClientMaxBodySize() != 0
				&& response_location_->getClientMaxBodySize() < request_body_.size()) {
			throw "413 REQUEST ERROR! Size BODY too big";
		}
	}

} //namespace webserver