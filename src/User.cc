#include "../include/User.hpp"
#include <unistd.h>
extern char **environ;

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
		if (request_.find(HEADER_CHUNKED_METHOD, 0) != std::string::npos) {
			is_chunked_ = true;
		} else if (request_.find(HEADER_CONTENT_LENGHT_MEHOD, 0) != std::string::npos) {
			is_content_length_ = true;
			std::string len_type = HEADER_CONTENT_LENGHT_MEHOD;
			size_t len_start = request_.find(len_type) + len_type.length();
			size_t len_end = request_.find(END_OF_LINE_CHUNKED, len_start);
			content_length_ = std::atoi(request_.substr(len_start, len_end - len_start).c_str());
		}
	}

	bool User::recvHeader() {
		if (request_.find(END_HEADER_HTPP, 0) != std::string::npos) {
			recv_header_ = true;
			checkBodyLenght();
			return true;
		}
		return false;
	}

	bool User::recvBody() {
		if (is_chunked_ && request_.find(END_BLOCK_IN_CHUNKED_METHOD, 0) == std::string::npos) {
			return false;
		} else if (is_content_length_ && request_.size() < request_.find(END_HEADER_HTPP, 0) + 4 + content_length_) {
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
			request_protocol_ = HTTP_VERSION_1_1;
			throw REQ_ERROR_400 "Wrong first line in reqest";
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
		if (msg[0] != METHOD_GET && msg[0] != METHOD_POST
				&& msg[0] != METHOD_PUT && msg[0] != METHOD_DELETE) {
			throw REQ_ERROR_405 "Wrong method in reqest";
		}
		if (msg[1].length() > 2048) {
			throw REQ_ERROR_414 "Wrong URI length";
		}
		if (msg[2] != HTTP_VERSION_1_1 && msg[2] != HTTP_VERSION_1_0) {
			throw REQ_ERROR_505 "Wrong protocol in reqest";
		}
	}

	void User::parseRequestHeader(const std::string& header_string) {
		std::string delimeter= HEADER_KEY_VALUE_DELIMETR;
		int find_delim = header_string.find(delimeter, 0);
		if (find_delim == std::string::npos) {
			throw REQ_ERROR_400 "Header mistake, no delimeter ':'";
		}
		int value_start = find_delim + delimeter.length();
		std::string key = header_string.substr(0, find_delim);
		if (key.size() > HEADER_KEY_LENGTH) {
			throw REQ_ERROR_400 "Header key too big";
		}
		std::string value = header_string.substr(value_start, header_string.size() - value_start);
		if (value.size() > HEADER_VALUE_LENGTH) {
			throw REQ_ERROR_400 "Header value too big";
		}
		request_header_.insert(std::pair<std::string, std::string>(key, value));
		if (request_header_.size() > HEADER_FIELD_SIZE) {
			throw REQ_ERROR_400 "Header fields too much";
		}
	}

	void User::recvBodyParseChunked() {
		std::string new_row = END_OF_LINE_CHUNKED;
		int len_new_row = new_row.length();
		int pars_position = request_.find(END_HEADER_HTPP) + 4;
		int header_len = request_.find(END_USEFUL_BLOOCK_IN_CHUNKED_METHOD);

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

	void User::recvBodyParseLength() {
		int body_start = request_.find(END_HEADER_HTPP, 0) + 4;
		if (request_.size() > body_start + content_length_ + 2)
			throw REQ_ERROR_400 "RECV size > then MUST BE";
		request_body_ = request_.substr(body_start, content_length_);
	}

	void User::parseRequestFindLocation() {
		std::map<std::string, Location>::const_iterator it_loc;
		std::string tmp_path = request_uri_;
		int n = 0;
		while (true) {
			it_loc = server_->getLocation().find(tmp_path);
			if (it_loc != server_->getLocation().end()) {
				if (!it_loc->second.getReturn().empty()) {
					if (server_->getLocation().find(it_loc->second.getReturn()) != server_->getLocation().end()) {
						it_loc = server_->getLocation().find(it_loc->second.getReturn());
						request_uri_ = request_uri_.substr(tmp_path.length());
						request_uri_ = it_loc->second.getPath() + "/" + request_uri_;
						clearDoubleSplash(request_uri_);
						tmp_path = request_uri_;
						n++;
						if (n == 5) {
							throw ERROR_508 "loop return in config";
						}
						continue ;
					}
				}
				break;
			}
			if (tmp_path == "/") {
				throw ERROR_404 "path not found parseRequestFindLocation";
			}
			tmp_path = tmp_path.substr(0, tmp_path.rfind('/'));
			if (tmp_path.empty()) {
				tmp_path = "/";
			}
		}
		response_location_ = &(it_loc->second);
	}

	void User::checkResponsePathUri() {
		response_path_ = server_->getRoot() + "/" + response_location_->getRoot() + "/" + request_uri_;
		clearDoubleSplash(response_path_);
		if (isDirectory(response_path_)){
			response_path_ += "/";
			response_dir_ = true;
		}
		if (isFile(response_path_)) {
			response_file_ = true;
		}
	}

	void User::createResponseReturnHeader() {
		status_code_ = STATUS_301_MOVED_PERMANENTLY;
		response_header_ += request_protocol_ + " " + status_code_ + " " + http_code_list_->find(status_code_)->second + END_OF_LINE_CHUNKED;
		response_header_ += HEADER_LOCATION + response_location_->getReturn() + END_OF_LINE_CHUNKED;
	}


	void User::CGIsetEnv() {
		setenv("CONTENT_LENGTH", std::to_string(content_length_).c_str(), 1);
		setenv("CONTENT_TYPE", "text/html", 1);
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
		setenv("PATH_INFO", request_uri_.c_str(), 1);
		setenv("REQUEST_URI", request_uri_.c_str(), 1);
		setenv("QUERY_STRING", request_query_.c_str(), 1);
		setenv("REQUEST_METHOD", request_method_.c_str(), 1);
		char buffer[16];
		inet_ntop( AF_INET, &addr_.sin_addr, buffer, sizeof(buffer));
		setenv("REMOTE_ADDR", buffer, 1);
		setenv("SCRIPT_NAME", response_location_->getCgiPath().c_str(), 1);
		setenv("SERVER_NAME", "webserv", 1);
		setenv("SERVER_PORT", std::to_string(server_->getPort()).c_str(), 1);
		setenv("SERVER_PROTOCOL", request_protocol_.c_str(), 1);
		setenv("SERVER_SOFTWARE", "WebServ/21.0", 1);
		setenv("AUTH_TYPE", "", 1);
		setenv("PATH_TRANSLATED", response_path_.c_str(), 1);
		setenv("REMOTE_IDENT", "", 1);
		setenv("REMOTE_USER", "", 1);
		setenv("REDIRECT_STATUS", STATUS_200_OK, 1);
		for (std::map<std::string, std::string>::iterator it = request_header_.begin(),
			ite = request_header_.end(); it != ite; ++it)
		{
			std::string tmp = it->first;
			for (int i = 0; i < tmp.size(); ++i) {
				tmp[i] = toupper(tmp[i]);
			}
			setenv(("HTTP_" + tmp).c_str(), it->second.c_str(), 1);
		}
	}

	void User::CGIparseBody() {
		size_t end;
		while ((end = response_body_.find(END_OF_LINE_CHUNKED)) != std::string::npos) {
			std::vector<std::string> cgi_body_split= split(response_body_.substr(0, end), " ");	
			if (!cgi_body_split.empty() && cgi_body_split[0] == HEADER_STATUS) {
				if (cgi_body_split[1] != STATUS_200_OK) {
					if (http_code_list_->find(cgi_body_split[1]) == http_code_list_->end()) {
						throw ERROR_500 "No error code find in CGI";
					}
					std::string error = cgi_body_split[1] + " Error from CGI";
					throw error.c_str();
				}
			} else if (!cgi_body_split.empty() && cgi_body_split[0] == "Content-Type:") {
				content_type_cgi_ = response_body_.substr(0, end);
			} else if (!cgi_body_split.empty() && cgi_body_split[0] == "Set-Cookie:") {
				response_header_cookie_ = response_body_.substr(0, end);
			}
			response_body_.erase(0, end + 2);
		}
		status_code_ = STATUS_200_OK;
	}

	void User::parseResponseCGI() {
	    int input = dup(STDIN_FILENO);
	    int output = dup(STDOUT_FILENO);

	    FILE *fsInput = tmpfile();
	    FILE *fsOutput = tmpfile();

	    int fdInput = fileno(fsInput);
	    int fdOutput = fileno(fsOutput);

	    write(fdInput, request_body_.c_str(), request_body_.size());
	    lseek(fdInput, 0, SEEK_SET);

		pid_t pid = fork();
		if (pid == -1) {
			throw ERROR_502 "Could not create process in CgiHandler";
		}
		if (pid == 0) {
			CGIsetEnv();

			char const	*cgi_info[3];
			cgi_info[0] = response_location_->getCgiPath().c_str();
			cgi_info[1] = response_path_.c_str();
			cgi_info[2] = NULL;

			dup2(fdInput, STDIN_FILENO);
			dup2(fdOutput, STDOUT_FILENO);
			if (execve(cgi_info[0], (char *const *)cgi_info, environ) == -1) {
				exit(1);
			}
		}
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			throw ERROR_500 "waitpid error";
		}
		if (WIFEXITED(status) && WEXITSTATUS(status)) {
			throw ERROR_502 "cgi status error";
		}
		lseek(fdOutput, 0, SEEK_SET);
		int size_buf = 65535;
		char buffer[size_buf];
		int ret;
		while ((ret = read(fdOutput, buffer, size_buf - 1)) != 0) {
			if (ret == -1) {
				throw ERROR_500 "Error read from tmpfile in CGI";
			}
			response_body_.append(buffer, ret);
		}
		dup2(STDIN_FILENO, input);
		dup2(STDOUT_FILENO, output);
		close(input);
		close(output);
		close(fdInput);
		close(fdOutput);
		fclose(fsInput);
		fclose(fsOutput);
		CGIparseBody();
	}


	void User::createResponseBodyFromFile(const std::string& body_path_to_file) {
		if (response_location_ && response_ext_ != "" && 
				response_location_->getCgiExt().find(response_ext_) != response_location_->getCgiExt().end()) {
			parseResponseCGI();
		} else {
			std::string buffer;
			std::ifstream open_file(body_path_to_file);
			if (!open_file.is_open()) {
				throw ERROR_500 " File Body Cannot Open";
			}
			while(std::getline(open_file, buffer)) {
				response_body_ += buffer + "\n";
			}
			open_file.close();
			status_code_ = STATUS_200_OK;
		}
	}

	void User::createResponseBodyDirectory(const std::string& path) {
		DIR				*dp;
		struct dirent	*di_struct;
		struct stat		file_stats;
		std::string		tmp_uri = request_uri_;

		if (tmp_uri.back() != '/') {
			tmp_uri += "/";
		}
		response_body_ += "<h1> PATH : " + tmp_uri + "</h1>";
		response_body_ += "<table>";
		response_body_ += "<tr> <th>File name</th> <th>File size</th> </tr>";
		if ((dp = opendir(path.data())) != NULL) {
			while ((di_struct = readdir(dp)) != NULL) {
				std::string tmp_path = path + "/" + di_struct->d_name;
				stat(tmp_path.data(), &file_stats);
				response_body_ += "<tr>";
				response_body_ += "<td><a href=\"" + tmp_uri;
				response_body_ += di_struct->d_name;
				if (S_ISDIR(file_stats.st_mode)) {
					response_body_ += "/";
				}
				response_body_ += "\">" + std::string(di_struct->d_name) + "</a></td>";
				response_body_ += "<td>";
				if (S_ISDIR(file_stats.st_mode)) {
					response_body_ += " dir ";
				} else {
					std::string size = std::to_string(static_cast<float>(file_stats.st_size) / 1000);
					size = size.substr(0, size.size() - 3);
					response_body_ +=  size + "Kb";
				}
				response_body_ += "</td>";
	            response_body_ += "</tr>";
	        }
	        closedir(dp);
	    }
	    response_body_ += "</table>";
	}

	void User::parseResponseGetBody() {
		if (response_dir_ == true && response_location_->getIndex() != ""
			&& isFile(response_path_ + response_location_->getIndex())) {
			response_path_ += response_location_->getIndex();
			response_dir_ = false;
			response_file_ = true;
		}
		if (response_file_ == true
			&& response_path_.rfind('.') != std::string::npos
			&& response_path_.rfind('.') > response_path_.rfind('/')) {
			response_ext_ = response_path_.substr(response_path_.rfind('.'));
		}
		if (response_location_->getAllowMethod().find(request_method_) == response_location_->getAllowMethod().end()) {
			throw RESP_ERROR_405 "Not Allow parseResponseGetBody";
		} else if (response_dir_ == true && response_location_->getAutoindex() == "on") {
			createResponseBodyDirectory(response_path_);
		} else if (response_file_ == true) {
			createResponseBodyFromFile(response_path_);
		} else {
			throw ERROR_404 "path not found parseResponseGetBody";
		}
	}

	void User::parseResponseUpload() {
		std::string upload_dir = server_->getRoot() + response_location_->getUploadPath();
		if (!isDirectory(upload_dir)) {
			throw ERROR_500 "no dir for upload in POST";
		}
		std::string boundary;
		std::string boundary_end;
		std::map<std::string, std::string>::iterator it_content = request_header_.find("Content-Type");
		if (it_content != request_header_.end()
			&& it_content->second.find("multipart/form-data") != std::string::npos) {
			if (it_content->second.find("boundary") == std::string::npos) {
				throw REQ_ERROR_400 "multipart/form-data have no boundery";
			}
			std::string name_boundary = "boundary=";
			int name_boundary_len = name_boundary.length();
			int position_bounery_start = it_content->second.find(name_boundary) + name_boundary_len;
			int position_bounery_end = it_content->second.find(END_OF_LINE_CHUNKED, position_bounery_start);
			boundary = "--" + it_content->second.substr(position_bounery_start, position_bounery_end - position_bounery_start);
			boundary_end = boundary + "--";
		} else {
			throw ERROR_500 "wrong form for upload";
		}
		std::string file_name;
		std::string path_file;
		if (request_body_.find("filename=", 0) == std::string::npos) {
			throw REQ_ERROR_400 "no File name in request";
		} else {
			std::string tmp_name = "filename=\"";
			int tmp_name_len = tmp_name.length();
			int position_filename_start = request_body_.find(tmp_name) + tmp_name_len;
			int position_filename_end = request_body_.find("\"", position_filename_start);
			file_name = request_body_.substr(position_filename_start, position_filename_end - position_filename_start);
			path_file = upload_dir + "/" + file_name;
	 	}

		request_body_ = request_body_.substr(request_body_.find(END_HEADER_HTPP) + 4);
		request_body_ = request_body_.substr(0, request_body_.find(boundary));

		std::ofstream tmp_file(path_file);
		if (!tmp_file.is_open()) {
			throw ERROR_500 "cannot creat file in POST";
		}
		tmp_file << request_body_;
		tmp_file.close();
		createResponseBodyDirectory(upload_dir);
		status_code_ = STATUS_200_OK;
	}

	void User::parseResponsePostBody(){
		std::string tmp_ext = "";
		if (request_uri_.rfind('.') != std::string::npos
			&& request_uri_.rfind('.') > request_uri_.rfind('/')) {
			tmp_ext = request_uri_.substr(request_uri_.rfind('.'));
		}	

		if (tmp_ext != "" && response_location_->getCgiExt().find(tmp_ext) != response_location_->getCgiExt().end()) {
			response_path_ = server_->getRoot() + response_location_->getRoot() + request_uri_;
			response_ext_ = tmp_ext;
			parseResponseCGI();
		} else {		
			if (response_file_ == true && response_path_.rfind('.') != std::string::npos 
				&& response_path_.rfind('.') > response_path_.rfind('/')){
				response_ext_ = response_path_.substr(response_path_.rfind('.'));
			}
			if (response_file_ == true && response_ext_ != "" && response_location_
				&& response_location_->getCgiExt().find(response_ext_) != response_location_->getCgiExt().end()) {
				parseResponseCGI();
			} else if (response_location_->getAllowMethod().find(request_method_) == response_location_->getAllowMethod().end()) {
				throw RESP_ERROR_405 "Not Allow parseResponsePostBody";
			} else if (response_location_->getUploadEnable() == "on") {
				parseResponseUpload();
			} else if (response_file_ == true) {
				createResponseBodyFromFile(response_path_);
			} else {
				parseResponseCGI();
			}
		}
	}

	void User::parseResponseDeleteBody() {
		if (response_location_->getAllowMethod().find(request_method_) == response_location_->getAllowMethod().end()) {
			throw RESP_ERROR_405 "Not Allow";
		} else if (response_dir_ == false && response_file_ == false) {
			throw  ERROR_404 "path not found parseResponseDeleteBody : ";
		} else if (access(response_path_.c_str(), W_OK) != 0) {
			throw ERROR_403 "no access";
		} else if (std::remove(response_path_.c_str()) != 0) {
			throw ERROR_500 "Delete Error";
		}
		response_body_ += "<html>\r\n<body>\r\n<h1>File deleted.</h1>\r\n</body>\r\n</html>\r\n";
		status_code_ = STATUS_200_OK;
	}

	void User::parseResponsePutBody() {
		if (response_location_->getAllowMethod().find(request_method_) == response_location_->getAllowMethod().end()) {
			throw RESP_ERROR_405 "Not Allow parseResponsePutBody";
		}
		if (response_location_->getUploadEnable() == "off") {
			throw REQ_ERROR_400 "not allow to upload";
		}
		if (request_uri_ == "/") {
			throw REQ_ERROR_400 "no file in PUT";
		} 
		std::string file_name = request_uri_.substr(request_uri_.rfind("/"));
		std::string upload_dir = server_->getRoot() + response_location_->getUploadPath();
		if (!isDirectory(upload_dir)) {
			throw ERROR_500 "no dir for upload in PUT";
		}
		std::string path_file = upload_dir + file_name;
		if (isFile(path_file)) {
			status_code_ = STATUS_204_NO_CONTENT;
		} else {
			status_code_ = STATUS_201_CREATED;
		}
		std::ofstream tmp_file(path_file);
		if (!tmp_file.is_open())
			throw ERROR_500 "cannot creat file in PUT"; 
		tmp_file << request_body_;
		tmp_file.close();
	}

	void User::createResponse() {
		if (!response_location_->getReturn().empty()) {
			createResponseReturnHeader();
		} else {
			checkResponsePathUri();
			if (request_method_ == METHOD_GET) {
				parseResponseGetBody();
			} else if (request_method_ == METHOD_POST) {
				parseResponsePostBody();
			} else if (request_method_ == METHOD_DELETE) {
				parseResponseDeleteBody();
			} else if (request_method_ == METHOD_DELETE) {
				parseResponsePutBody();
			} else {
				throw RESP_ERROR_405 "Not Allow";
			}
			response_header_ += request_protocol_ + " " + status_code_ + " " + http_code_list_->find(status_code_)->second + END_OF_LINE_CHUNKED;
		}
		if (content_type_cgi_ != "") {
			response_header_ += content_type_cgi_ + END_OF_LINE_CHUNKED;
		} else if (mime_ext_list_->find(response_ext_) != mime_ext_list_->end()) {
			response_header_ += "Content-Type: " + mime_ext_list_->find(response_ext_)->second + END_OF_LINE_CHUNKED;
		} else {
			response_header_ += "Content-Type: text/html; charset=utf-8\r\n";
		}
		if (!response_header_cookie_.empty()) {
			response_header_ += response_header_cookie_ + END_OF_LINE_CHUNKED;
		}
		response_header_ += "Version: " + request_protocol_ + END_OF_LINE_CHUNKED;
		response_header_ += "Connection: keep-alive\r\n";
		response_header_ += "Keep-Alive: timeout=10\r\n";
		response_header_ += HEADER_CONTENT_LENGHT_MEHOD + std::to_string(response_body_.length());
		response_header_ += END_HEADER_HTPP;
		response_ = response_header_ + response_body_;	
	}

	void User::checkAndParseRequest() {
		std::string new_row = END_OF_LINE_CHUNKED;
		int len_new_row = new_row.length();
		int pars_position = 0;
		int header_len = request_.find(END_HEADER_HTPP);

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
			throw  ERROR_413 "Size BODY too big";
		}
	}

	void User::createResponseError(std::string const &msg_error)
	{
		std::string num_error = msg_error.substr(0, msg_error.find(" "));		
		createResponseErrorBody(msg_error);

		response_header_ += request_protocol_ + " " + num_error + " " + http_code_list_->find(num_error)->second + END_OF_LINE_CHUNKED;
		response_header_ += "Version: " + request_protocol_ + END_OF_LINE_CHUNKED;
		response_header_ += "Content-Type: text/html; charset=utf-8\r\n";
		response_header_ += "Connection: keep-alive\r\n";
		response_header_ += "Keep-Alive: timeout=5\r\n";
		response_header_ += HEADER_CONTENT_LENGHT_MEHOD + std::to_string(response_body_.length());
		response_header_ += END_HEADER_HTPP;
		response_ =  response_header_ + response_body_;
	}

	void User::createResponseErrorBody(std::string const &msg_error)
	{
		int error = std::atoi(msg_error.substr(0, msg_error.find(" ")).c_str());
		std::map<int, std::string>::const_iterator it_error = server_->getErrorPage().find(error);

		if (it_error != server_->getErrorPage().end())
			createResponseBodyFromFile(it_error->second);
		else
			response_body_ = msg_error;
	}

} //namespace webserver