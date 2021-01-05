#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "common.h"

class Collect: public FileAction
{
public:
	BOOL Proceed(LPCTSTR full_file_name, LPCTSTR file_name);
	std::vector<std::tstring> file_names;
};