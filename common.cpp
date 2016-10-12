#include "common.h"
#include <shlobj.h>

BOOL IsDirectory(std::string file)
{
	return ((GetFileAttributes(file.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CrawlFolder(std::string first_dir, unsigned max_depth, unsigned cur_depth, FileAction* file_action)
{
	if(max_depth < cur_depth)
		return FALSE;
	if (!IsDirectory(first_dir))
		return FALSE;
	std::string strSearch;
	if(!first_dir.length())
	{
		DWORD dir_size = GetCurrentDirectory(0, NULL);
		TCHAR* cur_dir = new TCHAR[dir_size];
		GetCurrentDirectory(dir_size, cur_dir);
		strSearch = cur_dir;
		delete [] cur_dir;
	}
	else
		strSearch = first_dir;
	if(*strSearch.rbegin() != '\\')
		strSearch += '\\';
	std::string strPath = strSearch;
	strSearch += '*';
	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile(strSearch.c_str(), &fd);
	if(hSearch == INVALID_HANDLE_VALUE)
		return FALSE;
	BOOL res = FALSE;
	do
	{
		std::string strFullName = strPath;
		strFullName += fd.cFileName;
		if (IsDirectory(strFullName.c_str()))
		{
			if((!strcmp(fd.cFileName, ".")) || (!strcmp(fd.cFileName, "..")))
				continue;
			if (CrawlFolder(strFullName, max_depth, cur_depth + 1, file_action))
			{
				res = TRUE;
				break;
			}
		}
		else
			if(file_action->Proceed(strFullName.c_str(), fd.cFileName))
			{
				res = TRUE;
				break;
			}
	}while(FindNextFile(hSearch, &fd));
	FindClose(hSearch);
	return res;
}

BOOL Collect::Proceed(LPCTSTR full_file_name, LPCTSTR file_name)
{
	file_names.push_back(full_file_name);
	return FALSE;
}