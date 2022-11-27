#ifndef LOCATION_HPP
#define LOCATION_HPP

namespace webserver{
	class Location {
		private:
			std::string				path_;
			std::string				index_;
			std::set<std::string>	allow_method_;
			std::string				root_;
			std::string				autoindex_;
			std::string				upload_enable_;
			std::string				upload_path_;
			std::string				cgi_path_;
			std::set<std::string>	cgi_ext_;
			std::string				return_;
			int						client_max_body_size;
		
		public:
			Location() {}
			~Location(){}
			Location(const Location& rhs) { *this = rhs; }
			Location& operator=(const Location& rhs) {
				path_ = rhs.path_;
				index_ = rhs.index_;
				allow_method_ = rhs.allow_method_;
				root_ = rhs.root_;
				autoindex_ = rhs.autoindex_;
				upload_enable_ = rhs.upload_enable_;
				upload_path_ = rhs.upload_path_;
				cgi_path_ = rhs.cgi_path_;
				cgi_ext_ = rhs.cgi_ext_;
				return_ = rhs.return_;
				client_max_body_size = rhs.client_max_body_size;
				return *this;
			}

			void	init() {
				path_.clear();
				index_.clear();
				allow_method_.clear();
				root_.clear();
				autoindex_.clear();
				upload_enable_.clear();
				upload_path_.clear();
				cgi_ext_.clear();
				cgi_path_.clear();
				return_.clear();
				client_max_body_size = -1;
			}

			void	setPath(const std::string& repl_path)							{ path_ = repl_path; }
			void	setIndex(const std::string& repl_index)							{ index_ = repl_index; }
			void	setAllowMethod(const std::set<std::string>& repl_allow_method)	{ allow_method_ = repl_allow_method; }
			void	setRoot(const std::string& repl_root_)							{ root_ = repl_root_;}
			void	setAutoindex(const std::string& repl_autoindex)					{ autoindex_ = repl_autoindex;}
			void	setUploadEnable(const std::string& repl_upload_enable)			{ upload_enable_ = repl_upload_enable; }
			void	setUploadPath(const std::string& repl_upload_path)				{ upload_path_ = repl_upload_path; }
			void	setCgiPath(const std::string& repl_cgi_path)					{ cgi_path_ = repl_cgi_path; }
			void	setCgiExt(const std::set<std::string>& repl_cgi_ext)			{ cgi_ext_ = repl_cgi_ext; }
			void	setReturn(const std::string& repl_return)						{return_ = repl_return; }
			void	setClientMaxBodySize(const int& repl_client_max_body_size)		{ client_max_body_size = repl_client_max_body_size; }

			const std::string&	getPath() const										{ return path_; }
			const std::string&	getIndex() const									{ return index_; }
			const std::set<std::string>&	getAllowMethod() const					{ return allow_method_; }
			const std::string&	getRoot() const 									{ return root_; }
			const std::string&	getAutoindex() const								{ return autoindex_; }
			const std::string&	getUploadEnable() const								{ return upload_enable_; }
			const std::string&	getUploadPath() const								{ return upload_path_; }
			const std::string&	getCgiPath() const									{ return cgi_path_; }
			const std::set<std::string>&	getCgiExt() 						{ return cgi_ext_; }
			const std::string&	getReturn() const									{ return return_; }
			const int&	getClientMaxBodySize() const								{ return client_max_body_size; }

	}; //Location

} //server

#endif