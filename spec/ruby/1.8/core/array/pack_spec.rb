require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

if ENV['MRI'] then
  $: << 'kernel/core'
  require 'pack'
end

describe "Array#pack" do
  it "raises an ArgumentError with ('%')" do
    lambda { [].pack("%") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError on empty array" do
    ['A', 'a', 'B', 'b', 'C', 'c', 'D', 'd',
     'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h',
     'I', 'i', 'L', 'l', 'M', 'm', 'N', 'n',
     'Q', 'q', 'U', 'u','w', 'Z'].each { |pat|
       lambda { [].pack(pat) }.should raise_error(ArgumentError)
     }
  end

  it "skips everything till the end of schema string with ('#')" do
    ["abc", "def"].pack("A*#A10%").should == "abc"
  end

  it "skips everything till the end of schema line with ('#')" do
    ["abc", "def"].pack("A*#A10%\nA*").should == "abcdef"
  end

  it "returns space padded string with ('A<count>')" do
    ['abcde'].pack('A7').should == 'abcde  '
  end

  it "cuts string if its size greater than directive count with ('A<count>')" do
    ['abcde'].pack('A3').should == 'abc'
  end

  it "consider count = 1 if count omited with ('A')" do
    ['abcde'].pack('A').should == 'a'
  end

  it "returns empty string if count = 0 with ('A<count>')" do
    ['abcde'].pack('A0').should == ''
  end

  it "returns the whole argument string with star parameter with ('A')" do
    ['abcdef'].pack('A*').should == 'abcdef'
  end

  it "raises a TypeError if array item is not String with ('A<count>')" do
    lambda { [123].pack('A5') }.should raise_error(TypeError)
    lambda { [:hello].pack('A5') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('A5') }.should raise_error(TypeError)
  end

  it "returns null padded string with ('a<count>')" do
    ['abcdef'].pack('a7').should == "abcdef\x0"
  end

  it "cuts string if its size greater than directive count with ('a<count>')" do
    ['abcde'].pack('a3').should == 'abc'
  end

  it "considers count = 1 if count omited with ('a')" do
    ['abcde'].pack('a').should == 'a'
  end

  it "returns empty string if count = 0 with ('a<count>')" do
    ['abcde'].pack('a0').should == ''
  end

  it "returns the whole argument string with star parameter with ('a')" do
    ['abcdef'].pack('a*').should == 'abcdef'
  end

  it "raises a TypeError if array item is not String with ('a<count>')" do
    lambda { [123].pack('a5') }.should raise_error(TypeError)
    lambda { [:hello].pack('a5') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('a5') }.should raise_error(TypeError)
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

  it "conversion edge case: right one with ('B')" do
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

  it "raises a TypeError if corresponding array item is not String with ('B')" do
    lambda { [123].pack('B8') }.should raise_error(TypeError)
    lambda { [:data].pack('B8') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('B8') }.should raise_error(TypeError)
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

  it "conversion edge case: right one with ('b')" do
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

  it "raises a TypeError if corresponding array item is not String with ('b')" do
    lambda { [123].pack('b8') }.should raise_error(TypeError)
    lambda { [:data].pack('b8') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('b8') }.should raise_error(TypeError)
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

  it "converts float to integer and returns char with that number with ('C')" do
    [5.0].pack('C').should == [5].pack('C')
  end

  not_compliant_on :rubinius do
    it "calls to_i on symbol and returns char with that number with ('C')" do
      [:hello].pack('C').should == [:hello.to_i].pack('C')
    end
  end

  it "raises a TypeError if value is string with ('C')" do
    lambda { ["hello"].pack('C') }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given with ('C')" do
    [1, 2, 3].pack('C3').should == "\001\002\003"
    [1, 2, 3].pack('C2C1').should == "\001\002\003"
  end

  it "returns empty string if count = 0 with ('C')" do
    [1, 2, 3].pack('C0').should == ''
  end

  it "with star parameter processes all remaining array items with ('C')" do
    [1, 2, 3, 4, 5].pack('C*').should == "\001\002\003\004\005"
  end

  it "raises an ArgumentError if count is greater than array elements left with ('C')" do
    lambda { [1, 2].pack('C3') }.should raise_error(ArgumentError)
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

  it "converts float to integer and returns char with that number with ('c')" do
    [5.0].pack('c').should == [5].pack('c')
  end

  not_compliant_on :rubinius do
    it "calls to_i on symbol and returns char with that number with ('c')" do
      [:hello].pack('c').should == [:hello.to_i].pack('c')
    end
  end

  it "raises a TypeError if value is string with ('c')" do
    lambda { ["hello"].pack('c') }.should raise_error(TypeError)
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

  it "raises an ArgumentError if count is greater than array elements left with ('c')" do
    lambda { [1, 2].pack('c3') }.should raise_error(ArgumentError)
  end


  it "encodes a high-nibble hexadecimal string with ('H')" do
    ["41"].pack("H2").should == "A"
    ["61"].pack("H2").should == "a"
    ["7e"].pack("H2").should == "~"

    %w(41 31 2a).pack("H2H2H2").should == "A1*"
    %w(41312a).pack("H6").should == "A1*"
    %w(41312a).pack("H*").should == "A1*"
  end
  
  it "encodes a low-nibble hexadecimal string with ('h')" do
    ["14"].pack("h2").should == "A"
    ["16"].pack("h2").should == "a"
    ["e7"].pack("h2").should == "~"

    %w(14 13 a2).pack("h2h2h2").should == "A1*"
    %w(1413a2).pack("h6").should == "A1*"
    %w(1413a2).pack("h*").should == "A1*"
  end


  it "encodes a positive integer with ('i')" do
    [0].pack('i').should == "\000\000\000\000"
    [2**32-1].pack('i').should == "\377\377\377\377"
  end

  little_endian do
    it "encodes a positive integer in little-endian order with ('i')" do
      [1].pack('i').should == "\001\000\000\000"
    end
    
    it "encodes 4 positive integers in little-endian order with ('i4')" do
      [1,1234,2,2345].pack('i4').should == "\001\000\000\000\322\004\000\000\002\000\000\000)\t\000\000"
    end
    
    it "encodes remaining integers in little-endian order with ('i*')" do
      [1,1234,2].pack('i*').should == "\001\000\000\000\322\004\000\000\002\000\000\000"
    end
  end
  
  big_endian do
    it "encodes a positive integer in big-endian order with ('i')" do
      [1].pack('i').should == "\000\000\000\001"
    end
    
    it "encodes 4 positive integers in big-endian order with ('i4')" do
      [1,1234,2,2345].pack('i4').should == "\000\000\000\001\000\000\004\322\000\000\000\002\000\000\t)"
    end
    
    it "encodes remaining integers in big-endian order with ('i*')" do
      [1,1234,2].pack('i*').should == "\000\000\000\001\000\000\004\322\000\000\000\002"
    end
  end
  
  it "raises a RangeError when the positive integer is too big with ('i')" do
    lambda { [2**32].pack('i') }.should raise_error(RangeError)
  end

  it "encodes a negative integer with ('i')" do
    [-1].pack('i').should == "\377\377\377\377"
  end

  little_endian do
    it "encodes a negative integer in little-endian order with ('i')" do
      [-2].pack('i').should == "\376\377\377\377"
    end
  end

  big_endian do
    it "encodes a negative integer in big-endian order with ('i')" do
      [-2].pack('i').should == "\377\377\377\376"
    end
  end

  it "raises a RangeError when the negative integer is too big with ('l')" do
    lambda { [-2**32].pack('l') }.should raise_error(RangeError)
  end
 
  it "encodes a positive integer with ('l')" do
    [0].pack('l').should == "\000\000\000\000"
    [2**32-1].pack('l').should == "\377\377\377\377"
  end

  little_endian do
    it "encodes a positive integer in little-endian order with ('l')" do
      [1].pack('l').should == "\001\000\000\000"
    end
  end

  big_endian do
    it "encodes a positive integer in big-endian order with ('l')" do
      [1].pack('l').should == "\000\000\000\001"
    end
  end

  it "raises a RangeError when the positive integer is too big with ('l')" do
    lambda { [2**32].pack('l') }.should raise_error(RangeError)
  end

  it "encodes a negative integer with ('l')" do
    [-1].pack('l').should == "\377\377\377\377"
  end

  little_endian do
    it "encodes a negative integer in little-endian order with ('l')" do
      [-2].pack('l').should == "\376\377\377\377"
    end
  end

  big_endian do
    it "encodes a negative integer in big-endian order with ('l')" do
      [-2].pack('l').should == "\377\377\377\376"
    end
  end

  it "raises a RangeError when the negative integer is too big with ('l')" do
    lambda { [-2**32].pack('l') }.should raise_error(RangeError)
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

  it "raises a TypeError if corresponding array item is not string with ('m')" do
    lambda { [123].pack('m') }.should raise_error(TypeError)
    lambda { [:hello].pack('m') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('m') }.should raise_error(TypeError)
  end

  it "ignores count parameter with ('m')" do
    ["ABC", "DEF", "GHI"].pack('m0').should == ["ABC"].pack('m')
    ["ABC", "DEF", "GHI"].pack('m3').should == ["ABC"].pack('m')
  end

  it "ignores star parameter with ('m')" do
    ["ABC", "DEF", "GHI"].pack('m*').should == ["ABC"].pack('m')
  end

  it "encodes an integer in network order with ('n')" do
    [1234].pack('n').should == "\004\322"
  end

  it "encodes 4 integers in network order with ('n4')" do
    [1234,5678,9876,5432].pack('n4').should == "\004\322\026.&\224\0258"
  end

  it "encodes a long in network-order with ('N')" do
    [1000].pack('N').should == "\000\000\003\350"
    [-1000].pack('N').should == "\377\377\374\030"

    [65536].pack('N').should == "\000\001\000\000"
    [-65536].pack('N').should == "\377\377\000\000"
    # TODO: add bigger numbers
  end

#   it "encodes a long in network-order with ('N4')" do
#     [1234,5678,9876,5432].pack('N4').should == "\000\000\004\322\000\000\026.\000\000&\224\000\000\0258"
#     # TODO: bigger
#   end

  it "encodes a long in little-endian order with ('V')" do
    [1000].pack('V').should == "\350\003\000\000"
    [-1000].pack('V').should == "\030\374\377\377"

    [65536].pack('V').should == "\000\000\001\000"
    [-65536].pack('V').should == "\000\000\377\377"
    # TODO: add bigger numbers
  end

  it "encodes a short in little-endian order with ('v')" do
    [1000].pack('v').should == "\350\003"
    [-1000].pack('v').should == "\030\374"

    [65536].pack('v').should == "\000\000"
    [-65536].pack('v').should == "\000\000"
  end

  it "encodes a positive integer with ('s')" do
    [0].pack('s').should == "\000\000"
    [2**32-1].pack('s').should == "\377\377"
  end

  little_endian do
    it "encodes a positive integer in little-endian order with ('s')" do
      [1].pack('s').should == "\001\000"
    end
  end

  big_endian do
    it "encodes a positive integer in big-endian order with ('s')" do
      [1].pack('s').should == "\000\001"
    end
  end

  it "raises a RangeError when the positive integer is too big with ('s')" do
    lambda { [2**32].pack('s') }.should raise_error(RangeError)
  end

  it "encodes a negative integer with ('s')" do
    [-1].pack('s').should == "\377\377"
  end

  little_endian do
    it "encodes a negative integer in little-endian order with ('s')" do
      [-2].pack('s').should == "\376\377"
    end
  end

  big_endian do
    it "encodes a negative integer in big-endian order with ('s')" do
      [-2].pack('s').should == "\377\376"
    end
  end

  it "raises a RangeError when the negative integer is too big with ('s')" do
    lambda { [-2**32].pack('s') }.should raise_error(RangeError)
  end

  it "converts integers into UTF-8 encoded byte sequences with ('U')" do
    numbers = [0, 1, 15, 16, 127,
        128, 255, 256, 1024]
    numbers.each do |n|
      [n].pack('U').unpack('U').should == [n]
    end
    [0x7F, 0x7F].pack('U*').should == "\x7F\x7F"
    [262193, 4736, 191, 12, 107].pack('U*').should == "\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B"
    [2**16+1, 2**30].pack('U2').should == "\360\220\200\201\375\200\200\200\200\200"

    lambda { [].pack('U') }.should raise_error(ArgumentError)
    lambda { [1].pack('UU') }.should raise_error(ArgumentError)
    lambda { [2**32].pack('U') }.should raise_error(RangeError)
    lambda { [-1].pack('U') }.should raise_error(RangeError)
    lambda { [-5].pack('U') }.should raise_error(RangeError)
    lambda { [-2**32].pack('U') }.should raise_error(RangeError)
  end

  it "only takes as many elements as specified after ('U')" do
    [?a,?b,?c].pack('U2').should == "ab"
  end

  it "converts big integers into UTF-8 encoded byte sequences with ('U')" do 
    #these are actually failing on String#unpack
    #  they are not passing the 'utf8_regex_strict' test
    compliant_on :ruby, :jruby do
      numbers = [ 2048, 4096, 2**16 -1, 2**16, 2**16 + 1, 2**30]
      numbers.each do |n|
        [n].pack('U').unpack('U').should == [n]
      end
    end
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
    [( 1 * 4).chr].pack('u').should == "!!```\n"
    [(16 * 4).chr].pack('u').should == "!0```\n"
    [(25 * 4).chr].pack('u').should == "!9```\n"
    [(63 * 4).chr].pack('u').should == "!_```\n"
  end

  it "replaces spaces in encoded string with grave accent (`) char with ('u')" do
    [( 0*4).chr].pack('u').should == "!````\n"
  end

  it "returns empty string if source string is empty with ('u')" do
    [""].pack('u').should == ""
  end

  it "raises a TypeError if corresponding array item is not string with ('u')" do
    lambda { [123].pack('u') }.should raise_error(TypeError)
    lambda { [:hello].pack('u') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('u') }.should raise_error(TypeError)
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

  it "converts to BER-compressed integer with ('w')" do
    [0].pack('w').should == "\000"
    [1].pack('w').should == "\001"
    [9999].pack('w').should == "\316\017"
    [2**64].pack('w').should == "\202\200\200\200\200\200\200\200\200\000"
    lambda { [-1].pack('w') }.should raise_error(ArgumentError)
    lambda { [-2**256].pack('w') }.should raise_error(ArgumentError)
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

  it "raises an ArgumentError if count greater than already generated string length with ('X')" do
    lambda { ['abcdef'].pack('A6X7') }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if it is first directive with ('X')" do
    lambda { [].pack('X') }.should raise_error(ArgumentError)
  end

  it "doesn't increment the array index count with ('X')" do
    ['abcd','efgh'].pack('A4X2A4').should == 'abefgh'
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

  it "doesn't increment the array index count with ('x')" do
    ['abcd','efgh'].pack('A4x2A4').should == "abcd\000\000efgh"
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

  it "raises a TypeError if array item is not String with ('Z')" do
    lambda { [123].pack('Z5') }.should raise_error(TypeError)
    lambda { [:hello].pack('Z5') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('Z5') }.should raise_error(TypeError)
  end

  # Scenario taken from Mongrel's use of the SO_ACCEPTFILTER struct
  it "reuses last array element as often as needed to complete the string" do
    expected = "httpready" + ("\000" * 247)
    ['httpready', nil].pack('a16a240').should == expected
  end
end

#   def test_pack
#     $format = "c2x5CCxsdils_l_a6";
#     # Need the expression in here to force ary[5] to be numeric.  This avoids
#     # test2 failing because ary2 goes str->numeric->str and ary does not.
#     ary = [1,-100,127,128,32767,987.654321098 / 100.0,12345,123456,-32767,-123456,"abcdef"]
#     $x = ary.pack($format)
#     ary2 = $x.unpack($format)

#     assert_equal(ary.length, ary2.length)
#     assert_equal(ary.join(':'), ary2.join(':'))
#     assert_match(/def/, $x)

#     $x = [-1073741825]
#     assert_equal($x, $x.pack("q").unpack("q"))

#     $x = [-1]
#     assert_equal($x, $x.pack("l").unpack("l"))
#   end

#   def test_pack_N
#     assert_equal "\000\000\000\000", [0].pack('N')
#     assert_equal "\000\000\000\001", [1].pack('N')
#     assert_equal "\000\000\000\002", [2].pack('N')
#     assert_equal "\000\000\000\003", [3].pack('N')
#     assert_equal "\377\377\377\376", [4294967294].pack('N')
#     assert_equal "\377\377\377\377", [4294967295].pack('N')

#     assert_equal "\200\000\000\000", [2**31].pack('N')
#     assert_equal "\177\377\377\377", [-2**31-1].pack('N')
#     assert_equal "\377\377\377\377", [-1].pack('N')

#     assert_equal "\000\000\000\001\000\000\000\001", [1,1].pack('N*')
#     assert_equal "\000\000\000\001\000\000\000\001\000\000\000\001", [1,1,1].pack('N*')
#   end

#   def test_unpack_N
#     assert_equal 1, "\000\000\000\001".unpack('N')[0]
#     assert_equal 2, "\000\000\000\002".unpack('N')[0]
#     assert_equal 3, "\000\000\000\003".unpack('N')[0]
#     assert_equal 3, "\000\000\000\003".unpack('N')[0]
#     assert_equal 4294967295, "\377\377\377\377".unpack('N')[0]
#     assert_equal [1,1], "\000\000\000\001\000\000\000\001".unpack('N*')
#     assert_equal [1,1,1], "\000\000\000\001\000\000\000\001\000\000\000\001".unpack('N*')
#   end

#   def test_pack_U
#     assert_raises(RangeError) { [-0x40000001].pack("U") }
#     assert_raises(RangeError) { [-0x40000000].pack("U") }
#     assert_raises(RangeError) { [-1].pack("U") }
#     assert_equal "\000", [0].pack("U")
#     assert_equal "\374\277\277\277\277\277", [0x3fffffff].pack("U")
#     assert_equal "\375\200\200\200\200\200", [0x40000000].pack("U")
#     assert_equal "\375\277\277\277\277\277", [0x7fffffff].pack("U")
#     assert_raises(RangeError) { [0x80000000].pack("U") }
#     assert_raises(RangeError) { [0x100000000].pack("U") }
#   end
