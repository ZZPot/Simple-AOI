#include "files.h"
#include <shlobj.h>

BOOL Collect::Proceed(LPCTSTR full_file_name, LPCTSTR file_name)
{
	file_names.push_back(full_file_name);
	return FALSE;
}