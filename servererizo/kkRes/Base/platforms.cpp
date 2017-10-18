#ifdef WIN32
#include "stdafx.h"
#endif
#include <stdio.h>
#include "platforms.h"
#ifdef WIN32
void WindowsLOGE(const char* format,...)
{
	//return;
	char buf[1024]="";
	
	va_list ap;  
	va_start(ap,format);  
	vsprintf(buf,format,ap);  
	va_end(ap);  
	OutputDebugStringA(buf);  

}
#endif
#ifndef Platforms_H_
	#ifndef WIN32_KK
		
#ifndef WIN32
	   BOOL GetClientRect(HWND hWnd,RECT *lpRect)
	   {
            return TRUE;
	   } 
	   void assert(int i)
	   {

	   }
#endif
	  
#endif
#endif

