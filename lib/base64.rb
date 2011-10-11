#
# = base64.rb: methods for base64-encoding and -decoding stings
#
# Author:: Yukihiro Matsumoto 
# Documentation:: Dave Thomas and Gavin Sinclair
#
# Until Ruby 1.8.1, these methods were defined at the top-level.  Now
# they are in the Base64 module but included in the top-level, where
# their usage is deprecated.
#
# See Base64 for documentation.
#

#require "kconv"


# The Base64 module provides for the encoding (#encode64) and decoding
# (#decode64) of binary data using a Base64 representation.
# 
# The following particular features are also provided:
# - encode into lines of a given length (#b64encode)
# - decode the special format specified in RFC2047 for the
#   representation of email headers (decode_b)
#
# == Example
#
# A simple encoding and decoding. 
# 
#     require "base64"
#
#     enc   = Base64.encode64('Send reinforcements')
#                         # -> "U2VuZCByZWluZm9yY2VtZW50cw==\n" 
#     plain = Base64.decode64(enc)
#                         # -> "Send reinforcements"
#
# The purpose of using base64 to encode data is that it translates any
# binary data into purely printable characters.  It is specified in
# RFC 2045 (http://www.faqs.org/rfcs/rfc2045.html).

module Base64
  ENCODE_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
  DECODE_TABLE = { 'A' => 0, 'B' => 1, 'C' => 2, 'D' => 3, 'E' => 4, 'F' => 5, 'G' => 6, 
  'H' => 7, 'I' => 8, 'J' => 9, 'K' => 10, 'L' => 11, 'M' => 12, 'N' => 13, 'O' => 14, 
  'P' => 15, 'Q' => 16, 'R' => 17, 'S' => 18, 'T' => 19, 'U' => 20, 'V' => 21, 'W' => 22,
  'X' => 23, 'Y' => 24, 'Z' => 25, 'a' => 26, 'b' => 27, 'c' => 28, 'd' => 29, 'e' => 30,
  'f' => 31, 'g' => 32, 'h' => 33, 'i' => 34, 'j' => 35, 'k' => 36, 'l' => 37, 'm' => 38,
  'n' => 39, 'o' => 40, 'p' => 41, 'q' => 42, 'r' => 43, 's' => 44, 't' => 45, 'u' => 46, 
  'v' => 47, 'w' => 48, 'x' => 49, 'y' => 50, 'z' => 51, '0' => 52, '1' => 53, '2' => 54, 
  '3' => 55, '4' => 56, '5' => 57, '6' => 58, '7' => 59, '8' => 60, '9' => 61, '+' => 62, 
  '/' => 63 }

  module_function

  # Returns the Base64-decoded version of +str+.
  #
  #   require 'base64'
  #   str = 'VGhpcyBpcyBsaW5lIG9uZQpUaGlzIG' +
  #         'lzIGxpbmUgdHdvClRoaXMgaXMgbGlu' +
  #         'ZSB0aHJlZQpBbmQgc28gb24uLi4K'
  #   puts Base64.decode64(str)
  #
  # <i>Generates:</i>
  #
  #    This is line one
  #    This is line two
  #    This is line three
  #    And so on...

  def decode64(str)
    str.unpack("m")[0]
  end


  # Decodes text formatted using a subset of RFC2047 (the one used for
  # mime-encoding mail headers).
  #
  # Only supports an encoding type of 'b' (base 64), and only supports
  # the character sets ISO-2022-JP and SHIFT_JIS (so the only two
  # encoded word sequences recognized are <tt>=?ISO-2022-JP?B?...=</tt> and
  # <tt>=?SHIFT_JIS?B?...=</tt>).  Recognition of these sequences is case
  # insensitive.

  def decode_b(str)
    str.gsub!(/=\?ISO-2022-JP\?B\?([!->@-~]+)\?=/i) {
      decode64($1)
    }
    #str = Kconv::toeuc(str)
    str.gsub!(/=\?SHIFT_JIS\?B\?([!->@-~]+)\?=/i) {
      decode64($1)
    }
    #str = Kconv::toeuc(str)
    str.gsub!(/\n/, ' ') 
    str.gsub!(/\0/, '')
    str
  end

  # Returns the Base64-encoded version of +str+.
  #
  #    require 'base64'
  #    Base64.b64encode("Now is the time for all good coders\nto learn Ruby")
  #
  # <i>Generates:</i>
  #
  #    Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKdG8gbGVhcm4g
  #    UnVieQ==

  def encode64(bin)
    [bin].pack("m")
  end

  # _Prints_ the Base64 encoded version of +bin+ (a +String+) in lines of
  # +len+ (default 60) characters.
  #
  #    require 'base64'
  #    data = "Now is the time for all good coders\nto learn Ruby" 
  #    Base64.b64encode(data)
  #
  # <i>Generates:</i>
  #
  #    Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKdG8gbGVhcm4g
  #    UnVieQ==

  def b64encode(bin, len = 60)
    encode64(bin).scan(/.{1,#{len}}/) do
      print $&, "\n"
    end
  end 

  # Returns the Base64-encoded version of +bin+.
  # This method complies with RFC 4648.
  # No line feeds are added.
  
  # This does not currently work as base64 strict requires that
  # no newlines are inserted, and the current implementation of
  # pack("m0") in rubinius does match with MRI's logic
  #def strict_encode64(bin)
  #  [bin].pack("m0").chomp
  #end

  # Returns the Base64-decoded version of +str+.
  # This method complies with RFC 4648.
  # ArgumentError is raised if +str+ is incorrectly padded or contains
  # non-alphabet characters.  Note that CR or LF are also rejected.
  def strict_decode64(str)
    # no sense in doing all the magic below if it's just an empty string
    return str if str == ""
    # the base64 decoding algorithm operates in units of 4
    raise ArgumentError, "invalid base64" unless (str.length % 4) == 0
    decoded = ""

    # Operate on the string in units of 4
    0.step(str.length - 1, 4) do | i |
      # This holds values for byte mangling to produce the original string
      byte_buffer = Array.new
      # This will hold the result of the byte mangling on the above array values
      decode_buffer = Array.new

      # In Rubinius, str[i] will return the byte value while str[i..i]
      # gives the actual character, which is what we want
      first_character = str[i..i]; second_character = str[i+1..i+1];
      third_character = str[i+2..i+2]; fourth_character = str[i+3..i+3];

      # For the first two characters, verify they are in the decoding table
      # and append to the byte buffer accordingly
      raise ArgumentError, "character not in base64 alphabet" if !DECODE_TABLE.key?(first_character)
      byte_buffer << DECODE_TABLE[first_character]

      raise ArgumentError, "character not in base64 alphabet" if !DECODE_TABLE.key?(second_character)
      byte_buffer << DECODE_TABLE[second_character]

      # check for padding
      if third_character == "="
        # the padding is invalid if it's
        raise ArgumentError, "invalid base64 padding" if 
          # not at the end of the string
          (i + 4) != str.length or 
          # the last character is also not an = sign
          fourth_character != "=" or 
          # not a correct placeholder for a 0 byte
          (byte_buffer[1] & 0xf) != 0

        decode_buffer << (byte_buffer[0] << 2 | byte_buffer[1] >> 4)
        decoded << decode_buffer.pack('c*')
        return decoded
      end

      raise ArgumentError, "character not in base64 alphabet" if !DECODE_TABLE.key?(third_character)
      byte_buffer << DECODE_TABLE[third_character]

      if fourth_character == "="
        # same check logic as above
        raise ArgumentError, "invalid base64 padding" if 
          (i + 4) != str.length or 
          (byte_buffer[2] & 0x3) != 0

        decode_buffer << (byte_buffer[0] << 2 | byte_buffer[1] >> 4)
        decode_buffer << (byte_buffer[1] << 4 | byte_buffer[2] >> 2)
        decoded << decode_buffer.pack('c*')
        return decoded
      end

      raise ArgumentError, "character not in base64 alphabet" if !DECODE_TABLE.key?(fourth_character)
      byte_buffer << DECODE_TABLE[fourth_character]

      # here some bit operations happen which are then packed
      # to the original string value
      decode_buffer << (byte_buffer[0] << 2 | byte_buffer[1] >> 4)
      decode_buffer << (byte_buffer[1] << 4 | byte_buffer[2] >> 2)
      decode_buffer << (byte_buffer[2] << 6 | byte_buffer[3])
      decoded << decode_buffer.pack('c*')
    end

    decoded
  end

  # Returns the Base64-encoded version of +bin+.
  # This method complies with ``Base 64 Encoding with URL and Filename Safe
  # Alphabet'' in RFC 4648.
  # The alphabet uses '-' instead of '+' and '_' instead of '/'.
  
  # This will be fixed when strict_encode64 is properly implemented
  #def urlsafe_encode64(bin)
  #  strict_encode64(bin).tr("+/", "-_")
  #end

  # Returns the Base64-decoded version of +str+.
  # This method complies with ``Base 64 Encoding with URL and Filename Safe
  # Alphabet'' in RFC 4648.
  # The alphabet uses '-' instead of '+' and '_' instead of '/'.
  def urlsafe_decode64(str)
    strict_decode64(str.tr("-_", "+/"))
  end

end
