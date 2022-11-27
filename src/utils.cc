#include "../include/utils.hpp"
#include <algorithm>

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

} //webserver