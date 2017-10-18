#include <algorithm>
#include "KKStringTool.h"
static int ToUpper(int c)  
{  
    return toupper(c);  
} 
static int ToLower(int c)  
{  
    return tolower(c);  
}
void StrToLower(std::string &str)
{
	std::transform(str.begin(), str.end(),str.begin(), ToLower);
}