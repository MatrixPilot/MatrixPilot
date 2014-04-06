#include <stddef.h>
#include "stptok.h"

/******************************************************************
* DESCRIPTION:  You pass this function a string to parse,
*               a buffer to receive the "token" that gets scanned,
*               the length of the buffer, and a string of "break"
*               characters that stop the scan.
*               It will copy the string into the buffer up to
*               any of the break characters, or until the buffer
*               is full, and will always leave the buffer
*               null-terminated.  It will return a pointer to the
*               first non-breaking character after the one that
*               stopped the scan.
* RETURN:       It will return a pointer to the
*               first non-breaking character after the one that
*               stopped the scan or NULL on error or end of string.
* ALGORITHM:    none
******************************************************************/
char *stptok(
  const char *s,/* string to parse */
  char *tok, /* buffer that receives the "token" that gets scanned */
  size_t toklen,/* length of the buffer */
  const char *brk)/* string of break characters that will stop the scan */
{
  char *lim; /* limit of token */
  const char *b; /* current break character */


  /* check for invalid pointers */
  if (!s || !tok || !brk)
    return NULL;

  /* check for empty string */
  if (!*s)
    return NULL;

  lim = tok + toklen - 1;
  while ( *s && tok < lim )
  {
    for ( b = brk; *b; b++ )
    {
      if ( *s == *b )
      {
        *tok = 0;
        for (++s, b = brk; *s && *b; ++b)
        {
          if (*s == *b)
          {
            ++s;
            b = brk;
          }
        }
        if (!*s)
          return NULL;
        return (char *)s;
      }
    }
    *tok++ = *s++;
  }
  *tok = 0;

  if (!*s)
    return NULL;
  return (char *)s;
}

