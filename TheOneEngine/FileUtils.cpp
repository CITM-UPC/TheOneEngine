#include "FileUtils.h"
#include "SDL2/SDL.h"

std::string working_dir;
bool written_wd = false;

std::string Fileutils::GetWorkingDir() {
	if (!written_wd) {
		char* path = SDL_GetBasePath();
		working_dir = path;
		written_wd = true;
		SDL_free(path);
	}
	return working_dir;
}

void Fileutils::RemoveFileExtension(std::string& filename) {
	std::size_t pos = filename.find(".");    // position of "." in string
	filename = filename.substr(0, pos);
}

void Fileutils::NormalizePath(std::string& path, bool to_lowercase) {
	for (char &it : path) {
		if (it == '\\') {
			it = '/';
		}
		else if (to_lowercase) {
			it = tolower(it);
		}
	}
}

std::string Fileutils::GetNameFromPath(const std::string& path, bool include_extension) {
	std::string name = path;
	NormalizePath(name);
	name = name.substr(name.find_last_of('/') + 1);
	if (!include_extension) RemoveFileExtension(name);

	return name;
}
