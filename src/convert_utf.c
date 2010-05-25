/***************************************************************************
                          convert_utf.c

                             -------------------
    begin                :  Feb 08 2009
    copyright            : (C) 2009 by David Bruce
    email                : davidstuartbruce@gmail.com

    This file contains simple wrapper functions for converting wchar_t and
    UTF-8 strings using the GNU iconv library.

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "convert_utf.h"
#include "globals.h"

#include <stdio.h>
#include <string.h>
#include <SDL.h>

#define UTF_BUF_LENGTH 1024

/* GNU iconv()-based implementation:   */

int ConvertFromUTF8(wchar_t* wide_word, const char* UTF8_word, int max_length)
{
  wchar_t temp_wchar[UTF_BUF_LENGTH];
  wchar_t* wchar_start = temp_wchar;

  SDL_iconv_t conv_descr;
  size_t bytes_converted;
  size_t in_length = (size_t)UTF_BUF_LENGTH;
  size_t out_length = (size_t)UTF_BUF_LENGTH;

  if(max_length > UTF_BUF_LENGTH)
  {
    fprintf(stderr, "ConvertFromUTF8() - error - requested string length %d exceeds buffer length %d\n",
            max_length, UTF_BUF_LENGTH);
    return 0;
  }

  DEBUGMSG(debug_convert_utf, "ConvertFromUTF8(): UTF8_word = %s\n", UTF8_word);

  /* NOTE although we *should* be just able to pass "wchar_t" as the out_type, */
  /* iconv_open() segfaults on Windows if this is done - grrr....             */
#ifdef WIN32
  conv_descr = SDL_iconv_open("UTF-16LE", "UTF-8");
#else
  conv_descr = SDL_iconv_open("wchar_t", "UTF-8");
#endif

  /* NOTE casts to prevent compiler warnings */
  bytes_converted = SDL_iconv(conv_descr,
                          (char**)&UTF8_word, &in_length,
                          (char**)&wchar_start, &out_length);
  SDL_iconv_close(conv_descr);
  wcsncpy(wide_word, temp_wchar, max_length);

  DEBUGMSG(debug_convert_utf, "ConvertToUTF8(): wide_word = %S\n", wide_word);

  return wcslen(wide_word);
}


/* Now this uses GNU iconv and works correctly!   */
/* This probably could be simplified - not certain */
/* we have to copy into and out of the buffers     */

/******************To be used for savekeyboard*************/
/***Converts wchar_t string to char string*****************/
int ConvertToUTF8(const wchar_t* wide_word, char* UTF8_word, int max_length)
{
  char temp_UTF8[UTF_BUF_LENGTH];
  /* NOTE we need this because iconv_open() needs a char**.  We can't   */
  /* just pass "&temp_UTF8" because "temp_UTF8" is really a shorthand   */
  /* for "&temp_UTF8[0]", not its own memory location, so it doesn't    */
  /* have its own address. We ought to be able to do this directly into */
  /* into the argument UTF8_word string, but so far have had errors.    */
  char* UTF8_Start = temp_UTF8;

  SDL_iconv_t conv_descr;
  size_t bytes_converted;
  size_t in_length = (size_t)UTF_BUF_LENGTH;
  size_t out_length = (size_t)UTF_BUF_LENGTH;

  DEBUGMSG(debug_convert_utf, "ConvertToUTF8(): wide_word = %S\n", wide_word);

  if(max_length > UTF_BUF_LENGTH)
  {
    fprintf(stderr, "ConvertToUTF8() - error - requested string length %d exceeds buffer length %d\n",
            max_length, UTF_BUF_LENGTH);
    return 0;
  }

  /* NOTE although we *should* be just able to pass "wchar_t" as the in_type, */
  /* iconv_open() segfaults on Windows if this is done - grrr....             */
#ifdef WIN32
  conv_descr = SDL_iconv_open("UTF-8", "UTF-16LE");
#else
  conv_descr = SDL_iconv_open("UTF-8", "wchar_t");
#endif

  /* NOTE casts to prevent compiler warnings. While the documentation for iconv() */
  /* says arg 2 is a "const char**", it is "char**" in the iconv.h header itself. */
  bytes_converted = SDL_iconv(conv_descr,
                          (char**)&wide_word, &in_length,
                          (char**)&UTF8_Start, &out_length);
  SDL_iconv_close(conv_descr);
  strncpy(UTF8_word, temp_UTF8, max_length);

  DEBUGMSG(debug_convert_utf, "ConvertToUTF8(): UTF8_word = %s\n", UTF8_word);

  return strlen(UTF8_word);
}
