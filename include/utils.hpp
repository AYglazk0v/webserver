#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include <fstream>
# include <unistd.h>
# include <algorithm>
# include <sys/stat.h>
# include <arpa/inet.h>
# include <sys/types.h>

namespace webserver {

	std::vector<std::string>	split(std::string str, const std::string& delim);
	bool						isDirectory(const std::string& path);
	bool						isFile(const std::string& path);
	void						clearDoubleSplash(std::string& path);

} // namespace webserver

#endif