#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>

class FileAction
{
public:
	virtual ~FileAction(){};
	virtual BOOL Proceed(LPCTSTR full_file_name,
				LPCTSTR file_name) = 0;
};
BOOL CrawlFolder(std::string first_dir, unsigned max_depth, unsigned cur_depth, FileAction* file_action);

class Collect: public FileAction
{
public:
	BOOL Proceed(LPCTSTR full_file_name, LPCTSTR file_name);
	std::vector<std::string> file_names;
};