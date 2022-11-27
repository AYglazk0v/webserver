#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include <algorithm>
# include <sys/stat.h>
# include <sys/types.h>

namespace webserver {

	std::vector<std::string>	split(std::string str, const std::string& delim);
	bool						isDirectory(const std::string& path);
	void						clearDoubleSplash(std::string &test_path);

} // namespace webserver

#endif