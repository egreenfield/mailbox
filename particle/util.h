#ifndef _UTIL_H_
#define _UTIL_H_


inline void debug(char* fmt,...) {
  char buffer[256];
  va_list arg_ptr;
  va_start (arg_ptr, fmt); /* format string */
  vsprintf (buffer, fmt, arg_ptr);
  va_end (arg_ptr);

  Particle.publish("DEBUG",buffer);
}

#endif