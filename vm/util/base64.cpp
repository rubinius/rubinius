#include "util/base64.hpp"

/*
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

/* Modified by Evan Phoenix to be namespaced and slightly retyped */

#include "util/base64.hpp"

#include <stdint.h>

namespace base64 {

  static const uint8_t Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  static inline bool is_base64(uint8_t c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
  }

  std::string encode(uint8_t* bytes_to_encode, size_t in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    while (in_len--) {
      char_array_3[i++] = *(bytes_to_encode++);

      if(i == 3) {
        char_array_4[0] =  (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] =   char_array_3[2] & 0x3f;

        for(i = 0; (i <4) ; i++) {
          ret += Chars[char_array_4[i]];
        }

        i = 0;
      }
    }

    if(i) {
      for(j = i; j < 3; j++) {
        char_array_3[j] = '\0';
      }

      char_array_4[0] =  (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] =   char_array_3[2] & 0x3f;

      for (j = 0; (j < i + 1); j++) {
        ret += Chars[char_array_4[j]];
      }

      while((i++ < 3)) {
        ret += '=';
      }
    }

    return ret;
  }

  std::string decode(uint8_t* encoded_string, size_t len) {
    static int init = 0;
    static uint8_t xtable[256];

    size_t in_len = len;
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::string ret;

    /* Lazy init the decode table */
    if(!init) {
      init = 1;

      for(int k = 0; k < 256; k++) {
        xtable[k] = 255;
      }

      for(int k = 0; k < 64; k++) {
        xtable[Chars[k]] = k;
      }
    }

    while(in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
      char_array_4[i++] = encoded_string[in_++];

      if(i ==4) {
        for(i = 0; i <4; i++) {
          char_array_4[i] = xtable[char_array_4[i]];
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for(i = 0; (i < 3); i++) {
          ret += char_array_3[i];
        }

        i = 0;
      }
    }

    if(i) {
      for(j = i; j <4; j++) {
        char_array_4[j] = 0;
      }

      for(j = 0; j <4; j++) {
        char_array_4[j] = xtable[char_array_4[j]];
      }

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for(j = 0; (j < i - 1); j++) {
        ret += char_array_3[j];
      }
    }

    return ret;
  }
}

/*
namespace base64 {

  static const char tob64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  static inline uint8_t et(uint8_t i) {
    return (uint8_t)tob64[077 & i];
  }

  void encode_block(uint8_t* in, uint8_t* out, size_t len) {
    out[0] = et(in[0] >> 2);
    out[1] = et(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4));

    if(len > 1) {
      out[2] = et(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6));
    } else {
      out[2] = (uint8_t)'=';
    }

    if(len > 2) {
      out[3] = et(in[2] & 0x3f);
    } else {
      out[3] = (uint8_t)'=';
    }
  }


  size_t encode(uint8_t* in, uint8_t* out, size_t len) {

  }

  size_t decode(uint8_t* in, uint8_t* out, size_t len) {

  }
}

*/
