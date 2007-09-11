require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#pack" do
  #      Directive    Meaning
  #      ---------------------------------------------------------------
  #          @     |  Moves to absolute position
  #          B     |  Bit string (descending bit order)
  #          b     |  Bit string (ascending bit order)
  #          D, d  |  Double-precision float, native format
  #          E     |  Double-precision float, little-endian byte order
  #          e     |  Single-precision float, little-endian byte order
  #          F, f  |  Single-precision float, native format
  #          G     |  Double-precision float, network (big-endian) byte order
  #          g     |  Single-precision float, network (big-endian) byte order
  #          H     |  Hex string (high nibble first)
  #          h     |  Hex string (low nibble first)
  #          I     |  Unsigned integer
  #          i     |  Integer
  #          L     |  Unsigned long
  #          l     |  Long
  #          N     |  Long, network (big-endian) byte order
  #          n     |  Short, network (big-endian) byte-order
  #          P     |  Pointer to a structure (fixed-length string)
  #          p     |  Pointer to a null-terminated string
  #          Q, q  |  64-bit number
  #          S     |  Unsigned short
  #          s     |  Short
  #          U     |  UTF-8
  #          V     |  Long, little-endian byte order
  #          v     |  Short, little-endian byte order
  #          w     |  BER-compressed integer\fnm

  it "raises ArgumentError with ('%')" do
    should_raise(ArgumentError) { [].pack("%") }
  end

  it "skips everything till the end of schema string with ('#')" do
    ["abc", "def"].pack("A*#A10%").should == "abc"
  end

  it "skips everything till the end of schema line with ('#')" do
    ["abc", "def"].pack("A*#A10%\nA*").should == "abcdef"
  end

  it "returns space padded string with ('A')" do
    ['abcde'].pack('A7').should == 'abcde  '
  end

  it "cuts string if its size greater than directive count with ('A')" do
    ['abcde'].pack('A3').should == 'abc'
  end

  it "consider count = 1 if count omited with ('A')" do
    ['abcde'].pack('A').should == 'a'
  end

  it "returns empty string if count = 0 with ('A')" do
    ['abcde'].pack('A0').should == ''
  end

  it "returns the whole argument string with star parameter with ('A')" do
    ['abcdef'].pack('A*').should == 'abcdef'
  end

  it "raises TypeError if array item is not String with ('A')" do
    should_raise(TypeError) { [123].pack('A5') }
    should_raise(TypeError) { [:hello].pack('A5') }
    should_raise(TypeError) { [Object.new].pack('A5') }
  end

  it "should work with multi-digit padding sizes with ('A')" do
    ['abcdef'].pack('A10').should == "abcdef    "
  end

  it "returns null padded string with ('a')" do
    ['abcdef'].pack('a7').should == "abcdef\x0"
  end

  it "cuts string if its size greater than directive count with ('a')" do
    ['abcde'].pack('a3').should == 'abc'
  end

  it "considers count = 1 if count omited with ('a')" do
    ['abcde'].pack('a').should == 'a'
  end

  it "returns empty string if count = 0 with ('a')" do
    ['abcde'].pack('a0').should == ''
  end

  it "returns the whole argument string with star parameter with ('a')" do
    ['abcdef'].pack('a*').should == 'abcdef'
  end

  it "raises TypeError if array item is not String with ('a')" do
    should_raise(TypeError) { [123].pack('a5') }
    should_raise(TypeError) { [:hello].pack('a5') }
    should_raise(TypeError) { [Object.new].pack('a5') }
  end

  it "returns packed bit-string descending order with ('B')" do
    ["011000010110001001100011"].pack('B24').should == 'abc'
  end

  it "uses char codes to determine if bit is set or not with ('B')" do
    ["bccddddefgghhhijjkklllmm"].pack('B24').should == ["011000010110001001100011"].pack('B24')
  end

  it "conversion edge case: all zeros with ('B')" do
    ["00000000"].pack('B8').should == "\000"
  end

  it "conversion edge case: all ones with ('B')" do
    ["11111111"].pack('B8').should == "\377"
  end

  it "conversion edge case: left one with ('B')" do
    ["10000000"].pack('B8').should == "\200"
  end

  it "conversion edge case: left one with ('B')" do
    ["00000001"].pack('B8').should == "\001"
  end

  it "conversion edge case: edge sequences not in first char with ('B')" do
    ["0000000010000000000000011111111100000000"].pack('B40').should == "\000\200\001\377\000"
  end

  it "uses zeros if count is not multiple of 8 with ('B')" do
    ["00111111"].pack('B4').should == ["00110000"].pack('B8')
  end

  it "returns zero-char for each 2 of count that greater than string length with ('B')" do
    [""].pack('B6').should == "\000\000\000"
  end

  it "returns extra zero char if count is odd and greater than string length with ('B')" do
    [""].pack('B7').should == "\000\000\000\000"
  end

  it "starts new char if string is ended before char's 8 bits with ('B')" do
    ["0011"].pack('B8').should == "0\000\000"
  end

  it "considers count = 1 if no explicit count it given with ('B')" do
    ["10000000"].pack('B').should == ["10000000"].pack('B1')
    ["01000000"].pack('B').should == ["01000000"].pack('B1')
  end

  it "returns empty string if count = 0 with ('B')" do
    ["10101010"].pack('B0').should == ""
  end

  it "uses argument string length as count if count = * with ('B')" do
    ["00111111010"].pack('B*').should == ["00111111010"].pack('B11')
  end

  it "consumes only one array item with ('B')" do
    ["0011", "1111"].pack('B*').should == ["0011"].pack('B4')
    ["0011", "1011"].pack('B*B*').should == ["0011"].pack('B4') + ["1011"].pack('B4')
  end

  it "raises TypeError if corresponding array item is not String with ('B')" do
    should_raise(TypeError) { [123].pack('B8') }
    should_raise(TypeError) { [:data].pack('B8') }
    should_raise(TypeError) { [Object.new].pack('B8') }
  end

  it "returns packed bit-string descending order with ('b')" do
    ["100001100100011011000110"].pack('b24').should == 'abc'
  end

  it "conversion edge case: all zeros with ('b')" do
    ["00000000"].pack('b8').should == "\000"
  end

  it "conversion edge case: all ones with ('b')" do
    ["11111111"].pack('b8').should == "\377"
  end

  it "conversion edge case: left one with ('b')" do
    ["10000000"].pack('b8').should == "\001"
  end

  it "conversion edge case: left one with ('b')" do
    ["00000001"].pack('b8').should == "\200"
  end

  it "conversion edge case: edge sequences not in first char with ('b')" do
    ["0000000010000000000000011111111100000000"].pack('b40').should == "\000\001\200\377\000"
  end

  it "uses char codes to determine if bit is set or not with ('b')" do
    ["abbbbccddefffgghiijjjkkl"].pack('b24').should == ["100001100100011011000110"].pack('b24')
  end

  it "uses zeros if count is not multiple of 8 with ('b')" do
    ["00111111"].pack('b4').should == ["00110000"].pack('b8')
  end

  it "returns zero-char for each 2 of count that greater than string length with ('b')" do
    [""].pack('b6').should == "\000\000\000"
  end

  it "returns extra zero char if count is odd and greater than string length with ('b')" do
    [""].pack('b7').should == "\000\000\000\000"
  end

  it "starts new char if argument string is ended before char's 8 bits with ('b')" do
    ["0011"].pack('b8').should == "\f\000\000"
  end

  it "considers count = 1 if no explicit count it given with ('b')" do
    ["10000000"].pack('b').should == ["10000000"].pack('b1')
    ["01000000"].pack('b').should == ["01000000"].pack('b1')
  end

  it "returns empty string if count = 0 with ('b')" do
    ["10101010"].pack('b0').should == ""
  end

  it "uses argument string length as count if count = * with ('b')" do
    ["00111111010"].pack('b*').should == ["00111111010"].pack('b11')
  end

  it "consumes only one array item with ('b')" do
    ["0011", "1111"].pack('b*').should == ["0011"].pack('b4')
    ["0011", "1011"].pack('b*b*').should == ["0011"].pack('b4') + ["1011"].pack('b4')
  end

  it "raises TypeError if corresponding array item is not String with ('b')" do
    should_raise(TypeError) { [123].pack('b8') }
    should_raise(TypeError) { [:data].pack('b8') }
    should_raise(TypeError) { [Object.new].pack('b8') }
  end

  it "returns string with char of appropriate number with ('C')" do
    [49].pack('C').should == '1'
  end

  it "reduces value to fit in byte with ('C')" do
    [257].pack('C').should == "\001"
  end

  it "converts negative values to positive with ('C')" do
    [-1].pack('C').should == [255].pack('C')
    [-257].pack('C').should == [255].pack('C')
  end

failure :rubinius do
  it "converts float to integer and returns char with that number with ('C')" do
    [5.0].pack('C').should == [5].pack('C')
  end
end

  it "calls to_i on symbol and returns char with that number with ('C')" do
    [:hello].pack('C').should == [:hello.to_i].pack('C')
  end

  it "raises TypeErorr if value is string with ('C')" do
    should_raise(TypeError) { ["hello"].pack('C') }
  end

  it "processes count number of array elements if count given with ('C')" do
    [1, 2, 3].pack('C3').should == "\001\002\003"
  end

  it "returns empty string if count = 0 with ('C')" do
    [1, 2, 3].pack('C0').should == ''
  end

  it "with star parameter processes all remaining array items with ('C')" do
    [1, 2, 3, 4, 5].pack('C*').should == "\001\002\003\004\005"
  end

  it "raises ArgumentError if count is greater than array elements left with ('C')" do
    should_raise(ArgumentError) { [1, 2].pack('C3') }
  end

  it "returns string with char of appropriate number with ('c')" do
    [49].pack('c').should == '1'
  end

  it "reduces value to fit in byte with ('c')" do
    [257].pack('c').should == "\001"
  end

  it "converts negative values to positive with ('c')" do
    [-1].pack('c').should == [255].pack('C')
    [-257].pack('c').should == [255].pack('C')
  end

failure :rubinius do
  it "converts float to integer and returns char with that number with ('c')" do
    [5.0].pack('c').should == [5].pack('c')
  end
end

  it "calls to_i on symbol and returns char with that number with ('c')" do
    [:hello].pack('c').should == [:hello.to_i].pack('c')
  end

  it "raises TypeErorr if value is string with ('c')" do
    should_raise(TypeError) { ["hello"].pack('c') }
  end

  it "processes count number of array elements if count given with ('c')" do
    [1, 2, 3].pack('c3').should == "\001\002\003"
  end

  it "returns empty string if count = 0 with ('c')" do
    [1, 2, 3].pack('c0').should == ''
  end

  it "with star parameter processes all remaining array items with ('c')" do
    [1, 2, 3, 4, 5].pack('c*').should == "\001\002\003\004\005"
  end

  it "raises ArgumentError if count is greater than array elements left with ('c')" do
    should_raise(ArgumentError) { [1, 2].pack('c3') }
  end

  it "enocdes string with Qouted Printable encoding with ('M')" do
    ["ABCDEF"].pack('M').should == "ABCDEF=\n"
  end

  it "doesn't encode new line chars with ('M')" do
    ["\nA"].pack('M').should == "\nA=\n"
  end

  it "always appends soft line break at the end of encoded string with ('M')" do
    ["ABC"].pack('M')[-2, 2].should == "=\n"
  end

  it "appends soft line break after each 72 chars + 1 encoded char in encoded string with ('M')" do
    s = ["A"*150].pack('M')
    s[73, 2].should == "=\n"
    s[148, 2].should == "=\n"

    s = ["A"*72+"\001"].pack('M')
    s[75, 2].should == "=\n"
  end

  it "doesn't quote chars 32..60 and 62..126) with ('M')" do
    32.upto(60) do |i|
     [i.chr].pack('M').should == i.chr+"=\n"
    end

    62.upto(126) do |i|
     [i.chr].pack('M').should == i.chr+"=\n"
    end
  end

  it "quotes chars by adding equal sign and char's hex value with ('M')" do
    ["\001"].pack('M').should == "=01=\n"
  end

  it "quotes equal sign with ('M')" do
    ["="].pack('M').should == "=3D=\n"
  end

  it "doesn't quote \\t char with ('M')" do
    ["\t"].pack('M').should == "\t=\n"
  end

  it "returns empty string if source string is empty with ('M')" do
    [""].pack('M').should == ""
  end

  it "calls to_s on object to convert to string with ('M')" do
    class X; def to_s; "unnamed object"; end; end

    [123].pack('M').should == "123=\n"
    [:hello].pack('M').should == "hello=\n"
    [X.new].pack('M').should == "unnamed object=\n"
  end

  it "ignores count parameter with ('M')" do
    ["ABC", "DEF", "GHI"].pack('M0').should == ["ABC"].pack('M')
    ["ABC", "DEF", "GHI"].pack('M3').should == ["ABC"].pack('M')
  end

  it "ignores star parameter with ('M')" do
    ["ABC", "DEF", "GHI"].pack('M*').should == ["ABC"].pack('M')
  end

  it "encodes string with Base64 encoding with ('m')" do
    ["ABCDEF"].pack('m').should == "QUJDREVG\n"
  end

  it "converts series of 3-char sequences into four 4-char sequences with ('m')" do
    ["ABCDEFGHI"].pack('m').size.should == 4+4+4+1
  end

  it "fills chars with non-significant bits with '=' sign with ('m')" do
    ["A"].pack('m').should == "QQ==\n"
  end

  it "appends newline at the end of result string with ('m')" do
    ["A"].pack('m')[-1, 1].should == "\n"
  end

  it "appends newline after each 60 chars in result string with ('m')" do
    s = ["ABC"*31].pack('m')
    s[60, 1].should == "\n"
    s[121, 1].should == "\n"
  end

  it "encodes 6-bit char less than 26 with capital letters with ('m')" do
    [( 0*4).chr].pack('m').should == "AA==\n"
    [( 1*4).chr].pack('m').should == "BA==\n"

    [(25*4).chr].pack('m').should == "ZA==\n"
  end

  it "encodes 6-bit char from 26 to 51 with lowercase letters with ('m')" do
    [(26*4).chr].pack('m').should == "aA==\n"
    [(27*4).chr].pack('m').should == "bA==\n"

    [(51*4).chr].pack('m').should == "zA==\n"
  end

  it "encodes 6-bit char 62 with '+' with ('m')" do
    [(62*4).chr].pack('m').should == "+A==\n"
  end

  it "encodes 6-bit char 63 with '/' with ('m')" do
    [(63*4).chr].pack('m').should == "/A==\n"
  end

  it "returns empty string if source string is empty with ('m')" do
    [""].pack('m').should == ""
  end

  it "raises TypeError if corresponding array item is not string with ('m')" do
    should_raise(TypeError) { [123].pack('m') }
    should_raise(TypeError) { [:hello].pack('m') }
    should_raise(TypeError) { [Object.new].pack('m') }
  end

  it "ignores count parameter with ('m')" do
    ["ABC", "DEF", "GHI"].pack('m0').should == ["ABC"].pack('m')
    ["ABC", "DEF", "GHI"].pack('m3').should == ["ABC"].pack('m')
  end

  it "ignores star parameter with ('m')" do
    ["ABC", "DEF", "GHI"].pack('m*').should == ["ABC"].pack('m')
  end

  it "encodes string with UU-encoding with ('u')" do
    ["ABCDEF"].pack('u').should == "&04)#1$5&\n"
  end

  it "converts series of 3-char sequences into four 4-char sequences with ('u')" do
    ["ABCDEFGHI"].pack('u').size.should == 4+4+4+1+1
  end

  it "appends zero-chars to source string if string length is not multiple of 3 with ('u')" do
    ["A"].pack('u').should == "!00``\n"
  end

  it "appends newline at the end of result string with ('u')" do
    ["A"].pack('u')[-1, 1].should == "\n"
  end

  it "splits source string into lines with no more than 45 chars with ('u')" do
    s = ["A"*91].pack('u')
    s[61, 1].should == "\n"
    s[123, 1].should == "\n"
  end

  it "prepends encoded line length to each line with ('u')" do
    s = ["A"*50].pack('u')
    s[ 0].should == 45+32
    s[62].should ==  5+32
  end

  it "encodes 6-bit char with another char starting from char 32 with ('u')" do
    [( 1*4).chr].pack('u').should == "!!```\n"
    [(16*4).chr].pack('u').should == "!0```\n"
    [(25*4).chr].pack('u').should == "!9```\n"
    [(63*4).chr].pack('u').should == "!_```\n"
  end

  it "replaces spaces in encoded string with grave accent (`) char with ('u')" do
    [( 0*4).chr].pack('u').should == "!````\n"
  end

  it "returns empty string if source string is empty with ('u')" do
    [""].pack('u').should == ""
  end

  it "raises TypeError if corresponding array item is not string with ('u')" do
    should_raise(TypeError) { [123].pack('u') }
    should_raise(TypeError) { [:hello].pack('u') }
    should_raise(TypeError) { [Object.new].pack('u') }
  end

  it "ignores count parameter with ('u')" do
    ["ABC", "DEF", "GHI"].pack('u0').should == ["ABC"].pack('u')
    ["ABC", "DEF", "GHI"].pack('u3').should == ["ABC"].pack('u')
  end

  it "ignores star parameter with ('u')" do
    ["ABC", "DEF", "GHI"].pack('u*').should == ["ABC"].pack('u')
  end

  it "decreases result string by one char with ('X')" do
    ['abcdef'].pack('A4X').should == 'abc'
  end

  it "with count decreases result string by count chars with ('X')" do
    ['abcdef'].pack('A6X4').should == 'ab'
  end

  it "with zero count doesnt change result string with ('X')" do
    ['abcdef'].pack('A6X0').should == 'abcdef'
  end

  it "treats start parameter as zero count with ('X')" do
    ['abcdef'].pack('A6X*').should == 'abcdef'
  end

  it "raises ArgumentError if count greater than already generated string length with ('X')" do
    should_raise(ArgumentError) { ['abcdef'].pack('A6X7') }
  end

  it "raises ArgumentError if it is first directive with ('X')" do
    should_raise(ArgumentError) { [].pack('X') }
  end

  it "returns zero-char string with ('x')" do
    [].pack('x').should == "\000"
  end

  it "returns string of count zero chars with count and ('x')" do
    [].pack('x5').should == "\000\000\000\000\000"
  end

  it "returns empty string with count == 0 and ('x')" do
    [].pack('x0').should == ""
  end

  it "behaves like with count == 0 with star parameter and ('x')" do
    [].pack('x*').should == ""
  end

  it "returns null padded string with ('Z')" do
    ['abcdef'].pack('Z7').should == "abcdef\000"
  end

  it "cuts string if its size greater than directive count with ('Z')" do
    ['abcde'].pack('Z3').should == 'abc'
  end

  it "considers count = 1 if count omited with ('Z')" do
    ['abcde'].pack('Z').should == 'a'
  end

  it "returns empty string if count = 0 with ('Z')" do
    ['abcde'].pack('Z0').should == ''
  end

  it "returns the whole argument string plus null char with star parameter with ('Z')" do
    ['abcdef'].pack('Z*').should == "abcdef\000"
  end

  it "raises TypeError if array item is not String with ('Z')" do
    should_raise(TypeError) { [123].pack('Z5') }
    should_raise(TypeError) { [:hello].pack('Z5') }
    should_raise(TypeError) { [Object.new].pack('Z5') }
  end
end
