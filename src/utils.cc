#include "../include/utils.hpp"

namespace webserver {

	std::vector<std::string> split(const std::string str, const std::string& delim) {
		std::vector<std::string> ret_val;
		ret_val.reserve(std::count(str.begin(), str.end(), ' ') + 1);
		for (size_t start, end = 0; (start = str.find_first_not_of(delim, end)) != std::string::npos;) {
			end = str.find(delim, start);
			ret_val.push_back(str.substr(start, end - start));
		}
		return ret_val;
	}


	bool isDirectory(const std::string& path) {
		struct  stat s;
		if (stat(path.data(), &s) == 0) {
			return S_ISDIR(s.st_mode);
		}
		return false;
	}

	void clearDoubleSplash(std::string &path)
	{
		for (int i = path.size() - 1; i > 0; i--)
			if ((path[i] == '/' && path[i - 1] == '/') || path.back() == '/')
				path.erase(i, 1);
	}

} //webserver