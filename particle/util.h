#ifndef _UTIL_H_
#define _UTIL_H_

#include "application.h"

inline void debug(const char* cat,const char* fmt = NULL,...) {
  if(fmt == NULL) {
	  Particle.publish(cat);
  } else {
	  char buffer[256];
	  va_list arg_ptr;
	  va_start (arg_ptr, fmt); /* format string */
	  vsprintf (buffer, fmt, arg_ptr);
	  va_end (arg_ptr);

	  Particle.publish(cat,buffer);
	}
}

#endif