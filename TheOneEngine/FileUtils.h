#ifndef __FILEUTILS_H__
#define __FILEUTILS_H__

#include <string>

namespace Fileutils
{
std::string GetWorkingDir();
void RemoveFileExtension(std::string& filename);
void NormalizePath(std::string& path, bool to_lowercase = false);
std::string GetNameFromPath(const std::string& path, bool include_extension = false);

}


#endif