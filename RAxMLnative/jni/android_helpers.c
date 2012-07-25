#include "android_helpers.h"
/* some functions for the likelihood computation */

unsigned int KISS32() {
  static unsigned int 
    x = 123456789, 
    y = 362436069,
    z = 21288629,
    w = 14921776,
    c = 0;
  unsigned int t;
  x += 545925293;
  y ^= (y<<13); 
  y ^= (y>>17); 
  y ^= (y<<5);
  t = z + w + c; 
  z = w; 
  c = (t>>31); 
  w = t & 2147483647;
  return (x+y+w);
}

