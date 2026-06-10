#pragma once

#ifndef NULL
# define NULL 0
#endif

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif


#ifndef BOOL
 typedef int            BOOL;
#endif

#ifndef BYTE
 typedef unsigned char  BYTE;
#endif

#ifndef WORD
 typedef unsigned short WORD;
#endif

#ifndef DWORD
#ifdef _WINDOWS
 typedef unsigned long  DWORD;
#else
 typedef unsigned int  DWORD;
#endif
#endif

#ifndef INT
 typedef int            INT;
#endif


#ifndef delete_
# define delete_(x) if(x) { delete x; x=NULL; }
#endif

#ifndef delete_a
# define delete_a(x) if(x) { delete [] x; x=NULL; }
#endif

