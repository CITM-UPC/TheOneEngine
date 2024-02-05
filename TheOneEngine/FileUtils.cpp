#include "FileUtils.h"
#include "SDL2/SDL.h"
#include <filesystem>

namespace Fileutils
{

	std::string GetWorkingDir() {
		return std::filesystem::current_path().string() + "\\";
	}

	void RemoveFileExtension(std::string& filename) {
		std::size_t pos = filename.find(".");    // position of "." in string
		filename = filename.substr(0, pos);
	}

	void NormalizePath(std::string& path, bool to_lowercase) {
		for (char& it : path) {
			if (it == '\\') {
				it = '/';
			}
			else if (to_lowercase) {
				it = tolower(it);
			}
		}
	}

	std::string GetNameFromPath(const std::string& path, bool include_extension) {
		std::string name = path;
		NormalizePath(name);
		name = name.substr(name.find_last_of('/') + 1);
		if (!include_extension) RemoveFileExtension(name);

		return name;
	}

}
