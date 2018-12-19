#ifndef __shared__h
#define __shared__h

#ifdef __APPLE__
#define uint uint32_t
#define NO_HISTORY_LIST
#endif

#include <iostream>
#include <string>
#include <stdint.h>

inline int error(std::string err, int rc){
  std::cout << "ERROR!: " << err << std::endl;
  return rc;
}
inline int error(int err, int rc){
  std::cout << "Line: " << err << std::endl;
  return rc;
}
inline int is_big_endian(void)
{
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};

  return bint.c[0] == 1; 
}

#endif
