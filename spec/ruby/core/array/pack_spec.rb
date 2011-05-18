# -*- coding: ISO-8859-1 -*-
#             ~~~~~~~~~~
# Script encoding of this file should be neither ASCII-8BIT, US-ASCII nor UTF-8.
# This makes it easier to verify that Strings are converted into correct encodings.

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

# general behavior

describe "Array#pack" do
  it "returns a String" do
    ["abc", "def"].pack("A*").should be_kind_of(String)
  end

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

  it "sequentially processes each pack format, which consumes element in the array, and finally concatenates their result" do
    ["abc", 1, 2, 3, '01000001', 0x20].pack('Av2NB8c').should ==
      encode("a\x01\x00\x02\x00\x00\x00\x00\x03A ", "binary")
  end

  it "ignores white spaces" do
    ["abc", 1, 2, 3, '01000001', 0x20, 0x61].pack("A  \f   v2\tN\rB8\nc\vC").should ==
      encode("a\x01\x00\x02\x00\x00\x00\x00\x03A a", "binary")
  end

  it "skips everything till the end of line (LF) string with ('#')" do
    ["abc", "def"].pack("A*#A10%").should == "abc"
    ["abc", "def"].pack("A*#junk junk junk junk junk\nA10").should == "abcdef       "
    ["abc", "def"].pack("A*#junk junk junk junk junk\rA10").should == "abc"
  end

  ruby_version_is '1.8.8' do
    it "returns a tainted string when the format is tainted" do
      ["abcd", 0x20].pack("A3C".taint).tainted?.should be_true
    end

    it "returns a tainted string when the format is tainted even if the given format is empty" do
      ["abcd", 0x20].pack("".taint).tainted?.should be_true
    end
  end

  it "returns a tainted string when a pack argument is tainted" do
    ["abcd".taint, 0x20].pack("A3C").tainted?.should be_true
  end

  it "returns a not tainted string even if the array is tainted" do
    ["abcd", 0x20].taint.pack("A3C").tainted?.should be_false
  end

  ruby_version_is '1.9' do
    it "returns a untrusted string when the format is untrusted" do
      ["abcd", 0x20].pack("A3C".untrust).untrusted?.should be_true
    end

    it "returns a untrusted string when the format is untrusted even if the given format is empty" do
      ["abcd", 0x20].pack("".untrust).untrusted?.should be_true
    end

    it "returns a untrusted string when a pack argument is untrusted" do
      ["abcd".untrust, 0x20].pack("A3C").untrusted?.should be_true
    end

    it "returns a trusted string even if the array is untrusted" do
      ["abcd", 0x20].untrust.pack("A3C").untrusted?.should be_false
    end

    it "returns a string in encoding of common to the concatenated results" do
      ["\u{3042 3044 3046 3048}", 0x2000B].pack("A*U").encoding.should ==
        Encoding::ASCII_8BIT
      ["abcde\xd1", "\xFF\xFe\x81\x82"].pack("A*u").encoding.should ==
        Encoding::ASCII_8BIT
      ["abcde".encode(Encoding::US_ASCII), "\xFF\xFe\x81\x82"].pack("A*u").encoding.should ==
        Encoding::ASCII_8BIT
      # under discussion [ruby-dev:37294]
      ["\u{3042 3044 3046 3048}", 1].pack("A*N").encoding.should == Encoding::ASCII_8BIT
    end
  end

  # Scenario taken from Mongrel's use of the SO_ACCEPTFILTER struct
  it "reuses last array element as often as needed to complete the string" do
    expected = "httpready" + ("\000" * 247)
    ['httpready', nil].pack('a16a240').should == expected
  end
end

describe "Array#pack with the empty format" do
  it "returns an empty string" do
    [1, 2, 3, true].pack("").should == ""
  end

  ruby_version_is '1.9' do
    it "returns an empty String in US-ASCII" do
      [1, 2, 3, true].pack("").encoding.should == Encoding::US_ASCII
    end
  end
end

# string to string formats

describe "Array#pack with ASCII-string format", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a String" do
    ["abc"].pack(format).should be_kind_of(String)
  end

  it "cuts string if its size greater than directive count" do
    ['abcde'].pack(format(3)).should == 'abc'
  end

  it "considers count = 1 if count omited" do
    ['abcde'].pack(format).should == 'a'
  end

  it "returns empty string if count = 0 with" do
    ['abcde'].pack(format(0)).should == ''
  end

  it "returns the whole argument string with star parameter" do
    ['abcdef'].pack(format('*')).should == 'abcdef'
  end

  it "comsumres only one array item per a format" do
    ["abc", "def"].pack(format('*')).should == "abc"
    ["abc", "def"].pack(format('*')+format('*')).should == "abcdef"
  end

  it "tries to convert the pack argument to a String using #to_str" do
    obj = mock('to_str')
    obj.should_receive(:to_str).and_return("abc")
    [obj].pack(format).should == "a"
  end

  it "raises a TypeError if array item is not String with ('A<count>')" do
    lambda { [123].pack(format(5)) }.should raise_error(TypeError)
    lambda { [[]].pack(format(5)) }.should raise_error(TypeError)
    lambda { [mock('not string')].pack(format(5)) }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "treats a multibyte character just as a byte sequence" do
      s = "\u3042\u3044\u3046\u3048"
      [s].pack(format('*')).bytes.to_a.should == s.bytes.to_a
      [s].pack(format('3')).bytesize.should == 3

      # example of dummy encoding
      s = "\u3042".encode(Encoding::UTF_32BE)
      [s].pack(format('*')).bytes.to_a.should == s.bytes.to_a

      # example of stateful encoding
      s = "\u3042".encode(Encoding::ISO_2022_JP)
      [s].pack(format('*')).bytes.to_a.should == s.bytes.to_a
    end

    # This feature is under discussion - [ruby-dev:37278]
    it "returns result in ASCII-8BIT" do
      ["abcd"].pack(format).encoding.should == Encoding::ASCII_8BIT
      ["\u3042"].pack(format).encoding.should == Encoding::ASCII_8BIT
      ["\u3042".encode(Encoding::UTF_32BE)].pack(format).encoding.should ==
        Encoding::ASCII_8BIT
      ["\u3042".encode(Encoding::ISO_2022_JP)].pack(format).encoding.should ==
        Encoding::ASCII_8BIT
    end

    # This feature is under discussion - [ruby-dev:37278]
    it "cuts byte sequence even if it breaks a multibyte character" do
      ["\u3042"].pack(format).should == "\xe3".force_encoding('ascii-8bit')
      ["\u3042".encode(Encoding::UTF_32BE)].pack(format(2)).should == "\x00\x00"
      ["\u3042".encode(Encoding::ISO_2022_JP)].pack(format(4)).should == "\e$B$"
    end
  end
end

describe "Array#pack with format 'A'" do
  it_behaves_like "Array#pack with ASCII-string format", 'A'

  it "returns space padded string" do
    ['abcde'].pack('A7').should == 'abcde  '
  end
end

describe "Array#pack with format 'a'" do
  it_behaves_like "Array#pack with ASCII-string format", 'a'

  it "returns null padded string with ('a<count>')" do
    ['abcdef'].pack('a7').should == "abcdef\x0"
  end
end

describe "Array#pack with format 'Z'" do
  it_behaves_like "Array#pack with ASCII-string format", 'a'

  it "returns null padded string with ('a<count>')" do
    ['abcdef'].pack('a7').should == "abcdef\x0"
  end
end

describe "Array#pack with format 'B'" do
  it "returns packed bit-string descending order" do
    ["011000010110001001100011"].pack('B24').should == encode('abc', "binary")
  end

  # [ruby-dev:37279]
  it "accepts characters other than 0 or 1 for compatibility to perl" do
    lambda{ ["abbbbccddefffgghiijjjkkl"].pack('B24') }.should_not raise_error
  end
  ruby_version_is '1.9' do
    it "treats the pack argument as a byte sequence when its characters are other than 0 or 1" do
      ["\u3042"*8].pack('B*').length.should == "\u3042".bytesize * 8 / 8
    end
  end

  it "conversion edge case: all zeros" do
    ["00000000"].pack('B8').should == encode("\000", "binary")
  end

  it "conversion edge case: all ones" do
    ["11111111"].pack('B8').should == encode("\xFF", "binary")
  end

  it "conversion edge case: left one" do
    ["10000000"].pack('B8').should == encode("\x80", "binary")
  end

  it "conversion edge case: right one" do
    ["00000001"].pack('B8').should == encode("\x01", "binary")
  end

  it "conversion edge case: edge sequences not in first char" do
    ["0000000010000000000000011111111100000000"].pack('B40').should == encode("\x00\x80\x01\xFF\x00", "binary")
  end

  it "uses zeros if count is not multiple of 8" do
    ["00111111"].pack('B4').should == ["00110000"].pack('B8')
  end

  it "returns zero-char for each 2 of count that greater than string length" do
    [""].pack('B6').should == encode("\x00\x00\x00", "binary")
  end

  it "returns extra zero char if count is odd and greater than string length" do
    [""].pack('B7').should == encode("\x00\x00\x00\x00", "binary")
  end

  it "starts new char if string is ended before char's 8 bits" do
    ["0011"].pack('B8').should == encode("0\x00\x00", "binary")
  end

  it "considers count = 1 if no explicit count it given" do
    ["10000000"].pack('B').should == ["10000000"].pack('B1')
    ["01000000"].pack('B').should == ["01000000"].pack('B1')
  end

  it "returns empty string if count = 0" do
    ["10101010"].pack('B0').should == encode("", "binary")
  end

  it "uses argument string length as count if count = *" do
    ["00111111010"].pack('B*').should == ["00111111010"].pack('B11')
  end

  it "consumes only one array item per a format" do
    ["0011", "1111"].pack('B*').should == ["0011"].pack('B4')
    ["0011", "1011"].pack('B*B*').should == ["0011"].pack('B4') + ["1011"].pack('B4')
  end

  it "raises a TypeError if corresponding array item is not String" do
    lambda { [123].pack('B8') }.should raise_error(TypeError)
    lambda { [[]].pack('B8') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('B8') }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      ["01000001"].pack("B").encoding.should == Encoding::ASCII_8BIT # ASCII "A"
      ["11111111"].pack("B").encoding.should == Encoding::ASCII_8BIT # invalid as ASCII
      ["1111111010000000000000011000000000000010"].pack("B").encoding.should == Encoding::ASCII_8BIT # valid as UTF-8
    end
  end
end


describe "Array#pack with format 'b'" do
  it "returns packed bit-string descending order" do
    ["100001100100011011000110"].pack('b24').should == encode('abc', "binary")
  end

  it "conversion edge case: all zeros" do
    ["00000000"].pack('b8').should == encode("\x00", "binary")
  end

  it "conversion edge case: all ones" do
    ["11111111"].pack('b8').should == encode("\xFF", "binary")
  end

  it "conversion edge case: left one" do
    ["10000000"].pack('b8').should == encode("\x01", "binary")
  end

  it "conversion edge case: right one" do
    ["00000001"].pack('b8').should == encode("\x80", "binary")
  end

  it "conversion edge case: edge sequences not in first char" do
    ["0000000010000000000000011111111100000000"].pack('b40').should == encode("\x00\x01\x80\xFF\x00", "binary")
  end

  # [ruby-dev:37279]
  it "accepts characters other than 0 or 1 for compatibility to perl" do
    lambda{ ["abbbbccddefffgghiijjjkkl"].pack('b24') }.should_not raise_error
  end
  ruby_version_is '1.9' do
    it "treats the pack argument as a byte sequence when its characters are other than 0 or 1" do
      ["\u3042"*8].pack('b*').length.should == "\u3042".bytesize * 8 / 8
    end
  end

  it "uses zeros if count is not multiple of 8" do
    ["00111111"].pack('b4').should == ["00110000"].pack('b8')
  end

  it "returns zero-char for each 2 of count that greater than string length" do
    [""].pack('b6').should == encode("\x00\x00\x00", "binary")
  end

  it "returns extra zero char if count is odd and greater than string length" do
    [""].pack('b7').should == encode("\x00\x00\x00\x00", "binary")
  end

  it "starts new char if argument string is ended before char's 8 bits" do
    ["0011"].pack('b8').should == encode("\x0C\x00\x00", "binary")
  end

  it "considers count = 1 if no explicit count it given" do
    ["10000000"].pack('b').should == ["10000000"].pack('b1')
    ["01000000"].pack('b').should == ["01000000"].pack('b1')
  end

  it "returns empty string if count = 0" do
    ["10101010"].pack('b0').should == encode("", "binary")
  end

  it "uses argument string length as count if count = *" do
    ["00111111010"].pack('b*').should == ["00111111010"].pack('b11')
  end

  it "consumes only one array item per a format" do
    ["0011", "1111"].pack('b*').should == ["0011"].pack('b4')
    ["0011", "1011"].pack('b*b*').should == ["0011"].pack('b4') + ["1011"].pack('b4')
  end

  it "raises a TypeError if corresponding array item is not String" do
    lambda { [123].pack('b8') }.should raise_error(TypeError)
    lambda { [[]].pack('b8') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('b8') }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      ["10000010"].pack("b").encoding.should == Encoding::ASCII_8BIT # ASCII "A"
      ["11111111"].pack("b").encoding.should == Encoding::ASCII_8BIT # invalid as ASCII
      ["1111111010000000000000011000000000000010"].pack("b").encoding.should == Encoding::ASCII_8BIT # valid as UTF-8
    end
  end
end

describe "Array#pack with format 'H'" do
  it "encodes hexadecimal digits to byte sequence in the order of high-nibble first" do
    ["41"].pack("H2").should == encode("\x41", "binary")
    ["61"].pack("H2").should == encode("\x61", "binary")
    ["7e"].pack("H2").should == encode("\x7E", "binary")
    ["7E"].pack("H2").should == encode("\x7E", "binary")
    ["1"].pack("H").should == encode("\x10", "binary")
    ["7E1"].pack("H3").should == encode("\x7E\x10", "binary")
  end

  it "ignores rest of the pack argument when the argument is too long" do
    ["41424344"].pack('H2').should == encode("\x41", "binary")
    ["41424344"].pack('H4').should == encode("\x41\x42", "binary")
  end

  it "fills low-nibble of the last byte with 0 when count is odd" do
    ["41424344"].pack('H3').should == encode("\x41\x40", "binary")
    ["41424344"].pack('H5').should == encode("\x41\x42\x40", "binary")
  end

  it "fills the rest bytes with 0 if pack argument has insufficient length" do
    ["4142"].pack("H4").should == encode("\x41\x42", "binary")
    ["4142"].pack("H5").should == encode("\x41\x42\x00", "binary")
    ["4142"].pack("H6").should == encode("\x41\x42\x00", "binary")
    ["4142"].pack("H7").should == encode("\x41\x42\x00\x00", "binary")
  end

  ruby_bug("[ruby-dev:37283]", "1.8.7.73") do
    it "fills low-nibble of the last byte with 0 when count is odd even if pack argument has insufficient length" do
      ["414"].pack("H3").should == encode("\x41\x40", "binary")
      ["414"].pack("H4").should == encode("\x41\x40", "binary")
      ["414"].pack("H5").should == encode("\x41\x40\x00", "binary")
      ["414"].pack("H6").should == encode("\x41\x40\x00", "binary")
    end
  end

  it "considers count = 1 if count omited" do
    ['41'].pack('H').should == "\x40"
  end

  it "returns empty string if count = 0 with" do
    ['4142'].pack('H0').should == ''
  end

  it "returns the whole argument string with star parameter" do
    ['414243444546'].pack('H*').should == encode("\x41\x42\x43\x44\x45\x46", "binary")
  end

  it "consumes only one array item per a format" do
    %w(41 31 2a).pack("H2").should == encode("\x41", "binary")
    %w(41 31 2a).pack("H2H2H2").should == encode("\x41\x31\x2A", "binary")
    %w(41 31 2a).pack("H6").should == encode("\x41\x00\x00", "binary")
  end

  it "tries to convert the pack argument to a String using #to_str" do
    obj = mock('to_str')
    obj.should_receive(:to_str).and_return("41")
    [obj].pack('H2').should == "\x41"
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      ["41"].pack("H").encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with format 'h'" do
  it "encodes hexadecimal digits to byte sequence in the order of low-nibble first" do
    ["14"].pack("h2").should == encode("\x41", "binary")
    ["16"].pack("h2").should == encode("\x61", "binary")
    ["e7"].pack("h2").should == encode("\x7E", "binary")
    ["E7"].pack("h2").should == encode("\x7E", "binary")
    ["1"].pack("h").should == encode("\x01", "binary")
    ["E71"].pack("h3").should == encode("\x7E\x01", "binary")
  end

  it "ignores rest of the pack argument when the argument is too long" do
    ["14243444"].pack('h2').should == encode("\x41", "binary")
    ["14243444"].pack('h4').should == encode("\x41\x42", "binary")
  end

  it "fills low-nibble of the last byte with 0 when count is odd" do
    ["14243444"].pack('h3').should == encode("\x41\x02", "binary")
    ["14243444"].pack('h5').should == encode("\x41\x42\x03", "binary")
  end

  it "fills the rest bytes with 0 if pack argument has insufficient length" do
    ["1424"].pack("h4").should == encode("\x41\x42", "binary")
    ["1424"].pack("h5").should == encode("\x41\x42\x00", "binary")
    ["1424"].pack("h6").should == encode("\x41\x42\x00", "binary")
    ["1424"].pack("h7").should == encode("\x41\x42\x00\x00", "binary")
  end

  ruby_bug("[ruby-dev:37283]", "1.8.7.73") do
    it "fills high-nibble of the last byte with 0 when count is odd even if pack argument has insufficient length" do
      ["142"].pack("h3").should == encode("\x41\x02", "binary")
      ["142"].pack("h4").should == encode("\x41\x02", "binary")
      ["142"].pack("h5").should == encode("\x41\x02\x00", "binary")
      ["142"].pack("h6").should == encode("\x41\x02\x00", "binary")
    end
  end

  it "considers count = 1 if count omited" do
    ['14'].pack('h').should == "\x01"
  end

  it "returns empty string if count = 0 with" do
    ['1424'].pack('h0').should == ''
  end

  it "returns the whole argument string with star parameter" do
    ['142434445464'].pack('h*').should == encode("\x41\x42\x43\x44\x45\x46", "binary")
  end

  it "consumes only one array item per a format" do
    %w(14 13 a2).pack("h2").should == encode("\x41", "binary")
    %w(14 13 a2).pack("h2h2h2").should == encode("\x41\x31\x2A", "binary")
    %w(14 13 a2).pack("h6").should == encode("\x41\x00\x00", "binary")
  end

  it "tries to convert the pack argument to a String using #to_str" do
    obj = mock('to_str')
    obj.should_receive(:to_str).and_return("14")
    [obj].pack('h2').should == "\x41"
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      ["14"].pack("h").encoding.should == Encoding::ASCII_8BIT
    end
  end
end




#
# shared behaviours for integer formats
#

describe "Array#pack with integer format which can not have platform dependent width", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "raises ArgumentError when tails suffix '_'" do
    lambda{ [1].pack("#{format}_") }.should raise_error(ArgumentError)
  end

  it "raises ArgumentError when tails suffix '!'" do
    lambda{ [1].pack("#{format}!") }.should raise_error(ArgumentError)
  end
end

describe "Array#pack with integer format (8bit)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string with byte of appropriate number" do
    [49].pack(format).should == encode('1', "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF", "binary")
    [-(2**7)].pack(format).should == encode("\x80", "binary")
  end

  it "reduces value to fit in byte" do
    [2**8-1].pack(format).should == encode("\xFF", "binary")
    [2**8  ].pack(format).should == encode("\x00", "binary")
    [2**8+1].pack(format).should == encode("\x01", "binary")

    [-2**8+1].pack(format).should == encode("\x01", "binary")
    [-2**8  ].pack(format).should == encode("\x00", "binary")
    [-2**8-1].pack(format).should == encode("\xFF", "binary")
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x05", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x05", "binary")
  end

  not_compliant_on :rubinius do
    ruby_version_is '' ... '1.9' do
      it "accepts a Symbol as a pack argument because it responds to #to_int" do
        [:hello].pack(format).should == [:hello.to_i].pack('C')
      end
    end
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode("\x01\x02\x03", "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode("\x01\x02\x03", "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should == encode("\x01\x02\x03\x04\x05", "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xE3, 0x81, 0x82].pack(format(3)).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (16bit, little endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 2 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00", "binary")
    [0xABCD].pack(format).should == encode("\xCD\xAB", "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF", "binary")
    [-2**15].pack(format).should == encode("\x00\x80", "binary")
  end

  it "drops higher bytes when a pack argument is >= 2**16" do
    [2**16-1].pack(format).should == encode("\xFF\xFF", "binary")
    [2**16  ].pack(format).should == encode("\x00\x00", "binary")
    [2**16+1].pack(format).should == encode("\x01\x00", "binary")
  end
  it "drops higher bytes when a pack argument is < -2**16" do
    [-2**16+1].pack(format).should == encode("\x01\x00", "binary")   # 0x ..F 00 01
    [-2**16 ].pack(format).should == encode("\x00\x00", "binary")    # 0x ..F 00 00
    [-2**16-1].pack(format).should == encode("\xFF\xFF", "binary")   # 0x .FE FF FF
  end

  ruby_version_is '' ... '1.9' do
    platform_is :wordsize => 32 do
      it "may raise a RangeError when a pack argument is >= 2**32" do
        lambda { [2**32-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**32].pack(format) }.should raise_error(RangeError)
      end

      it "may raise a RangeError when a pack argument is <= -2**32" do
        lambda { [-2**32+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**32].pack(format) }.should raise_error(RangeError)
      end
    end
    platform_is :wordsize => 64 do
      it "may raise a RangeError when a pack argument is >= 2**64" do
        lambda { [2**64-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**64].pack(format) }.should raise_error(RangeError)
      end

      it "may raise a RangeError when a pack argument is <= -2**64" do
        lambda { [-2**64+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**64].pack(format) }.should raise_error(RangeError)
      end
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    platform_is :wordsize => 32 do
      it "does not raise a RangeError even when a pack argument is >= 2**32" do
        [2**32-1].pack(format).should == encode("\xFF\xFF", "binary")
        [2**32  ].pack(format).should == encode("\x00\x00", "binary")
        [2**32+1].pack(format).should == encode("\x01\x00", "binary")
      end

      it "does not raise a RangeError even when a pack argument is <= -2**32" do
        [-2**32+1].pack(format).should == encode("\x01\x00", "binary")
        [-2**32  ].pack(format).should == encode("\x00\x00", "binary")
        [-2**32-1].pack(format).should == encode("\xFF\xFF", "binary")
      end
    end
    platform_is :wordsize => 64 do
      it "does not raise a RangeError even when a pack argument is >= 2**64" do
        [2**64-1].pack(format).should == encode("\xFF\xFF", "binary")
        [2**64  ].pack(format).should == encode("\x00\x00", "binary")
        [2**64+1].pack(format).should == encode("\x01\x00", "binary")
      end

      it "does not raise a RangeError even when a pack argument is <= -2**64" do
        [-2**64+1].pack(format).should == encode("\x01\x00", "binary")
        [-2**64  ].pack(format).should == encode("\x00\x00", "binary")
        [-2**64-1].pack(format).should == encode("\xFF\xFF", "binary")
      end
    end
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x05\x00", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x05\x00", "binary")
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode("\x01\x00\x02\x00\x03\x00", "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode("\x01\x00\x02\x00\x03\x00", "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should ==
      encode("\x01\x00\x02\x00\x03\x00\x04\x00\x05\x00", "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (16bit, big endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 2 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00", "binary")
    [0xABCD].pack(format).should == encode("\xAB\xCD", "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF", "binary")
    [-2**15].pack(format).should == encode("\x80\x00", "binary")
  end

  it "drops higher bytes when a pack argument is >= 2**32" do
    [2**16-1].pack(format).should == encode("\xFF\xFF", "binary")
    [2**16  ].pack(format).should == encode("\x00\x00", "binary")
    [2**16+1].pack(format).should == encode("\x00\x01", "binary")
  end

  it "drops higher bytes when a pack argument is < -2**32" do
    [-2**16+1].pack(format).should == encode("\x00\x01", "binary") # 0x ..F 00 01
    [-2**16 ].pack(format).should == encode("\x00\x00", "binary")  # 0x ..F 00 00
    [-2**16-1].pack(format).should == encode("\xFF\xFF", "binary") # 0x .FE FF FF
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x00\x05", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x00\x05", "binary")
  end

  ruby_version_is '' ... '1.9' do
    platform_is :wordsize => 32 do
      it "may raise a RangeError when a pack argument is >= 2**32" do
        lambda { [2**32-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**32].pack(format) }.should raise_error(RangeError)
      end

      it "may raise a RangeError when a pack argument is <= -2**32" do
        lambda { [-2**32+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**32].pack(format) }.should raise_error(RangeError)
      end
    end
    platform_is :wordsize => 64 do
      it "may raise a RangeError when a pack argument is >= 2**64" do
        lambda { [2**64-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**64].pack(format) }.should raise_error(RangeError)
      end

      it "may raise a RangeError when a pack argument is <= -2**64" do
        lambda { [-2**64+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**64].pack(format) }.should raise_error(RangeError)
      end
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    platform_is :wordsize => 32 do
      it "does not raise a RangeError even when a pack argument is >= 2**32" do
        [2**32-1].pack(format).should == encode("\xFF\xFF", "binary")
        [2**32  ].pack(format).should == encode("\x00\x00", "binary")
        [2**32+1].pack(format).should == encode("\x00\x01", "binary")
      end

      it "does not raise a RangeError even when a pack argument is <= -2**32" do
        [-2**32+1].pack(format).should == encode("\x00\x01", "binary")
        [-2**32  ].pack(format).should == encode("\x00\x00", "binary")
        [-2**32-1].pack(format).should == encode("\xFF\xFF", "binary")
      end
    end
    platform_is :wordsize => 64 do
      it "does not raise a RangeError even when a pack argument is >= 2**64" do
        [2**64-1].pack(format).should == encode("\xFF\xFF", "binary")
        [2**64  ].pack(format).should == encode("\x00\x00", "binary")
        [2**64+1].pack(format).should == encode("\x00\x01", "binary")
      end

      it "does not raise a RangeError even when a pack argument is <= -2**64" do
        [-2**64+1].pack(format).should == encode("\x00\x01", "binary")
        [-2**64  ].pack(format).should == encode("\x00\x00", "binary")
        [-2**64-1].pack(format).should == encode("\xFF\xFF", "binary")
      end
    end
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode("\x00\x01\x00\x02\x00\x03", "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode("\x00\x01\x00\x02\x00\x03", "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should ==
      encode("\x00\x01\x00\x02\x00\x03\x00\x04\x00\x05", "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (32bit, little endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 4 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00\x00\x00", "binary")
    [0xABCDEF01].pack(format).should == encode("\x01\xEF\xCD\xAB", "binary")
  end

  ruby_version_is '' ... '1.9' do
    platform_is :wordsize => 32 do
      it "raises a RangeError when a pack argument is >= 2**32" do
        lambda { [2**32-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**32].pack(format) }.should raise_error(RangeError)
      end

      it "raises a RangeError when a pack argument is <= -2**32" do
        lambda { [-2**32+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**32].pack(format) }.should raise_error(RangeError)
      end
    end
    platform_is :wordsize => 64 do
      it "drops higher bytes when a pack argument is >= 2**32" do
        [2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
        [2**32  ].pack(format).should == encode("\x00\x00\x00\x00", "binary")
        [2**32+1].pack(format).should == encode("\x01\x00\x00\x00", "binary")
      end

      it "drops higher bytes when a pack argument is < -2**32" do
        [-2**32+1].pack(format).should == encode("\x01\x00\x00\x00", "binary") # 0x ..F 00 00 00 01
        [-2**32 ].pack(format).should == encode("\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00
        [-2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF
      end
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    it "drops higher bytes when a pack argument is >= 2**32" do
      [2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
      [2**32  ].pack(format).should == encode("\x00\x00\x00\x00", "binary")
      [2**32+1].pack(format).should == encode("\x01\x00\x00\x00", "binary")
    end

    it "drops higher bytes when a pack argument is < -2**32" do
      [-2**32+1].pack(format).should == encode("\x01\x00\x00\x00", "binary") # 0x ..F 00 00 00 01
      [-2**32 ].pack(format).should == encode("\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00
      [-2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF
    end
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
    [-2**31].pack(format).should == encode("\x00\x00\x00\x80", "binary")
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x05\x00\x00\x00", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x05\x00\x00\x00", "binary")
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode("\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00", "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode("\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00", "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should ==
      encode("\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05\x00\x00\x00", "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (32bit, big endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 4 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00\x00\x00", "binary")
    [0xABCDEF01].pack(format).should == encode("\xAB\xCD\xEF\x01", "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
    [-2**31].pack(format).should == encode("\x80\x00\x00\x00", "binary")
  end

  ruby_version_is '' ... '1.9' do
    platform_is :wordsize => 32 do
      it "raises a RangeError when a pack argument is >= 2**32" do
        lambda { [2**32-1].pack(format) }.should_not raise_error(RangeError)
        lambda { [2**32].pack(format) }.should raise_error(RangeError)
      end

      it "raises a RangeError when a pack argument is <= -2**32" do
        lambda { [-2**32+1].pack(format) }.should_not raise_error(RangeError)
        lambda { [-2**32].pack(format) }.should raise_error(RangeError)
      end
    end
    platform_is :wordsize => 64 do
      it "drops higher bytes when a pack argument is >= 2**32" do
        [2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
        [2**32  ].pack(format).should == encode("\x00\x00\x00\x00", "binary")
        [2**32+1].pack(format).should == encode("\x00\x00\x00\x01", "binary")
      end

      it "drops higher bytes when a pack argument is < -2**32" do
        [-2**32+1].pack(format).should == encode("\x00\x00\x00\x01", "binary") # 0x ..F 00 00 00 01
        [-2**32 ].pack(format).should == encode("\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00
        [-2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF
      end
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    it "drops higher bytes when a pack argument is >= 2**32" do
      [2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary")
      [2**32  ].pack(format).should == encode("\x00\x00\x00\x00", "binary")
      [2**32+1].pack(format).should == encode("\x00\x00\x00\x01", "binary")
    end

    it "drops higher bytes when a pack argument is < -2**32" do
      [-2**32+1].pack(format).should == encode("\x00\x00\x00\x01", "binary") # 0x ..F 00 00 00 01
      [-2**32 ].pack(format).should == encode("\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00
      [-2**32-1].pack(format).should == encode("\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF
    end
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x00\x00\x00\x05", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x00\x00\x00\x05", "binary")
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode("\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03", "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode("\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03", "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should ==
      encode("\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05", "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (64bit, little endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 8 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
    [0xABCDEF0123456789].pack(format).should == encode("\x89\x67\x45\x23\x01\xEF\xCD\xAB", "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary")
    [-2**63].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x80", "binary")
  end

  ruby_version_is '' ... '1.9' do
    it "raises a RangeError when a pack argument is >= 2**64" do
      lambda { [2**64-1].pack(format) }.should_not raise_error(RangeError)
      lambda { [2**64].pack(format) }.should raise_error(RangeError)
    end

    it "raises a RangeError when a pack argument is <= -2**64" do
      lambda { [-2**64+1].pack(format) }.should_not raise_error(RangeError)
      lambda { [-2**64].pack(format) }.should raise_error(RangeError)
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    it "drops higher bytes when a pack argument is >= 2**64" do
      [2**64-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary")
      [2**64  ].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
      [2**64+1].pack(format).should == encode("\x01\x00\x00\x00\x00\x00\x00\x00", "binary")
    end

    it "drops higher bytes when a pack argument is < -2**64" do
      [-2**64+1].pack(format).should == encode("\x01\x00\x00\x00\x00\x00\x00\x00", "binary") # 0x ..F 00 00 00 00 00 00 00 01
      [-2**64 ].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00 00 00 00 00
      [-2**64-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF FF FF FF FF
    end
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x05\x00\x00\x00\x00\x00\x00\x00", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x05\x00\x00\x00\x00\x00\x00\x00", "binary")
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode(
      "\x01\x00\x00\x00\x00\x00\x00\x00" +
      "\x02\x00\x00\x00\x00\x00\x00\x00" +
      "\x03\x00\x00\x00\x00\x00\x00\x00" , "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode(
      "\x01\x00\x00\x00\x00\x00\x00\x00" +
      "\x02\x00\x00\x00\x00\x00\x00\x00" +
      "\x03\x00\x00\x00\x00\x00\x00\x00" , "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should == encode(
      "\x01\x00\x00\x00\x00\x00\x00\x00" +
      "\x02\x00\x00\x00\x00\x00\x00\x00" +
      "\x03\x00\x00\x00\x00\x00\x00\x00" +
      "\x04\x00\x00\x00\x00\x00\x00\x00" +
      "\x05\x00\x00\x00\x00\x00\x00\x00" , "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with integer format (64bit, big endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string containing 8 bytes for an integer" do
    [0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
    [0xABCDEF0123456789].pack(format).should == encode("\xAB\xCD\xEF\x01\x23\x45\x67\x89", "binary")
  end

  it "regards negative values as 2's complement in order to converts it to positive" do
    [-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary")
    [-2**63].pack(format).should == encode("\x80\x00\x00\x00\x00\x00\x00\x00", "binary")
  end

  ruby_version_is '' ... '1.9' do
    it "raises a RangeError when a pack argument is >= 2**64" do
      lambda { [2**64-1].pack(format) }.should_not raise_error(RangeError)
      lambda { [2**64].pack(format) }.should raise_error(RangeError)
    end

    it "raises a RangeError when a pack argument is <= -2**64" do
      lambda { [-2**64+1].pack(format) }.should_not raise_error(RangeError)
      lambda { [-2**64].pack(format) }.should raise_error(RangeError)
    end
  end
  # feature changed by MRI r5542 - [ruby-dev:22654].
  ruby_version_is '1.9' do
    it "drops higher bytes when a pack argument is >= 2**64" do
      [2**64-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary")
      [2**64  ].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
      [2**64+1].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x01", "binary")
    end

    it "drops higher bytes when a pack argument is < -2**64" do
      [-2**64+1].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x01", "binary") # 0x ..F 00 00 00 00 00 00 00 01
      [-2**64 ].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")  # 0x ..F 00 00 00 00 00 00 00 00
      [-2**64-1].pack(format).should == encode("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", "binary") # 0x .FE FF FF FF FF FF FF FF FF
    end
  end

  it "tries to convert the pack argument to an Integer using #to_int" do
    [5.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x05", "binary")

    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(5)
    [obj].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x05", "binary")
  end

  it "raises a TypeError if a pack argument can't be coerced to Integer" do
    lambda { ["5"].pack(format) }.should raise_error(TypeError)

    obj = mock('not an integer')
    lambda { [obj].pack(format) }.should raise_error(TypeError)
  end

  it "processes count number of array elements if count given" do
    [1, 2, 3].pack(format(3)).should == encode(
      "\x00\x00\x00\x00\x00\x00\x00\x01" +
      "\x00\x00\x00\x00\x00\x00\x00\x02" +
      "\x00\x00\x00\x00\x00\x00\x00\x03" , "binary")
    [1, 2, 3].pack(format(2) + format(1)).should == encode(
      "\x00\x00\x00\x00\x00\x00\x00\x01" +
      "\x00\x00\x00\x00\x00\x00\x00\x02" +
      "\x00\x00\x00\x00\x00\x00\x00\x03" , "binary")
  end

  it "returns empty string if count = 0" do
    [1, 2, 3].pack(format(0)).should == encode('', "binary")
  end

  it "with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack(format('*')).should == encode(
      "\x00\x00\x00\x00\x00\x00\x00\x01" +
      "\x00\x00\x00\x00\x00\x00\x00\x02" +
      "\x00\x00\x00\x00\x00\x00\x00\x03" +
      "\x00\x00\x00\x00\x00\x00\x00\x04" +
      "\x00\x00\x00\x00\x00\x00\x00\x05" , "binary")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [1, 2].pack(format(3)) }.should raise_error(ArgumentError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [0x41].pack(format).encoding.should == Encoding::ASCII_8BIT
      [0xFF].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end



# integer formats which has platform independent width and fixed byte order

describe "Array#pack with format 'C'" do
  it_behaves_like "Array#pack with integer format (8bit)", 'C'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'C'
end
describe "Array#pack with format 'c'" do
  it_behaves_like "Array#pack with integer format (8bit)", 'c'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'c'
end

describe "Array#pack with format 'n'" do
  it_behaves_like "Array#pack with integer format (16bit, big endian)", 'n'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'n'
end
describe "Array#pack with format 'v'" do
  it_behaves_like "Array#pack with integer format (16bit, little endian)", 'v'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'v'
end

describe "Array#pack with format 'N'" do
  it_behaves_like "Array#pack with integer format (32bit, big endian)", 'N'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'N'
end
describe "Array#pack with format 'V'" do
  it_behaves_like "Array#pack with integer format (32bit, little endian)", 'V'
  it_behaves_like "Array#pack with integer format which can not have platform dependent width", 'V'
end


# integer formats which has platform independent width but endian dependent

describe "Array#pack with format 's'" do
  big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's'    }
  little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's' }
end
describe "Array#pack with format 'S'" do
  big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 'S'    }
  little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 'S' }
end

describe "Array#pack with format 'l'" do
  big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'l'    }
  little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'l' }
end
describe "Array#pack with format 'L'" do
  big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'L'    }
  little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'L' }
end

describe "Array#pack with format 'q'" do
  big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'q'    }
  little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'q' }
end
describe "Array#pack with format 'Q'" do
  big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'Q'    }
  little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'Q' }
end


# integer formats which has platform depended width

# signed short int
describe "Array#pack with format 's!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's!'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's!' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there an architecture whose short int is 32bit width?
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's!'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's!' }
  end
end
describe "Array#pack with format 's_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's_'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's_' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there an architecture whose short int is 32bit width?
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's_'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's_' }
  end
end
# unsigned short int
describe "Array#pack with format 'S!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there an architecture whose short int is 32bit width?
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 's'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 's' }
  end
end
describe "Array#pack with format 'S_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 'S_'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 'S_' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there an architecture whose short int is 32bit width?
    big_endian    { it_behaves_like "Array#pack with integer format (16bit, big endian)", 'S_'    }
    little_endian { it_behaves_like "Array#pack with integer format (16bit, little endian)", 'S_' }
  end
end

# signed int
describe "Array#pack with format 'i'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i' }
  end
end
describe "Array#pack with format 'i!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i!' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i!' }
  end
end
describe "Array#pack with format 'i_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i_' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'i_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'i_' }
  end
end

# unsigned int
describe "Array#pack with format 'I'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I' }
  end
end
describe "Array#pack with format 'I!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I!' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I!' }
  end
end
describe "Array#pack with format 'I_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I_' }
  end
  platform_is :wordsize => 64 do
    # TODO: supporting ILP64 architechture
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'I_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'I_' }
  end
end

# signed long
describe "Array#pack with format 'l!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'l!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'l!' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there anything other LLP64 platform which ruby can run on?
    platform_is :os => :mswin do
      not_compliant_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'l!'    }
        little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'l!' }
      end
      deviates_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'l!'    }
        little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'l!' }
      end
    end
    platform_is_not :os => :mswin do
      big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'l!'    }
      little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'l!' }
    end
  end
end
describe "Array#pack with format 'l_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'l_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'l_' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there anything other LLP64 platform which ruby can run on?
    platform_is :os => :mswin do
      not_compliant_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'l_'    }
        little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'l_' }
      end
      deviates_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'l_'    }
        little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'l_' }
      end
    end
    platform_is_not :os => :mswin do
      big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'l_'    }
      little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'l_' }
    end
  end
end

# unsigned long
describe "Array#pack with format 'L!'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'L!'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'L!' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there anything other LLP64 platform which ruby can run on?
    platform_is :os => :mswin do
      not_compliant_on :jruby do
        # I'm not sure that this is sensible behavior for MRI,
        # being 64bit but treating long as 32 bit, and only on Windows.
        big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'L!'    }
        little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'L!' }
      end
      deviates_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'L!'    }
        little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'L!' }
      end
    end
    platform_is_not :os => :mswin do
      big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'L!'    }
      little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'L!' }
    end
  end
end
describe "Array#pack with format 'L_'" do
  platform_is :wordsize => 32 do
    big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'L_'    }
    little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'L_' }
  end
  platform_is :wordsize => 64 do
    # TODO: Is there anything other LLP64 platform which ruby can run on?
    platform_is :os => :mswin do
      not_compliant_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (32bit, big endian)", 'L_'    }
        little_endian { it_behaves_like "Array#pack with integer format (32bit, little endian)", 'L_' }
      end
      deviates_on :jruby do
        big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'L_'    }
        little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'L_' }
      end
    end
    platform_is_not :os => :mswin do
      big_endian    { it_behaves_like "Array#pack with integer format (64bit, big endian)", 'L_'    }
      little_endian { it_behaves_like "Array#pack with integer format (64bit, little endian)", 'L_' }
    end
  end
end



# floating point number formats
describe "Array#pack with float format", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "consumes as many Floats as specified" do
    [1.0, :a, :b, :c].pack(format).should == [1.0].pack(format)
    [1.0, 2.0, :a, :b, :c].pack(format(2)).should == [1.0].pack(format) + [2.0].pack(format)
    [1.0, 2.0, 3.0, :a, :b, :c].pack(format(2)+format).should == [1.0].pack(format) + [2.0].pack(format) + [3.0].pack(format)
  end

  it "tries to convert the pack argument to a Float using #to_f" do
    # 1.9 requires that the object actually be Numeric before calling #to_f on
    # it; 1.8 doesn't care.
    num = mock_numeric('number')
    num.should_receive(:to_f).and_return(1.5)
    lambda{ [num].pack(format) }.should_not raise_error
  end

  ruby_version_is ""..."1.9" do
    it "accepts a string representation of real number as the pack argument" do
      lambda{ ["1.3333"].pack(format) }.should_not raise_error(TypeError)
      lambda{ ["-1.3333"].pack(format) }.should_not raise_error(TypeError)
    end
  end

  it "accepts an integer as the pack argument" do
    lambda{ [1].pack(format) }.should_not raise_error(TypeError)
    lambda{ [0x80000000].pack(format) }.should_not raise_error(TypeError)
  end

  it "does not raise an error even if passed a integer which is too large to cast into float value" do
    lambda{ [2**1024].pack(format) }.should_not raise_error
  end

  ruby_version_is '' ... '1.8' do
    it "may complain overflow when the passed integer is too large" do
      lambda{ [2**1024].pack(format) }.should complain(/range/)
    end
  end

  it "raises a TypeError if corresponding array item is not Float" do
    lambda { [nil].pack(format) }.should raise_error(TypeError)
    lambda { [:hello].pack(format) }.should raise_error(TypeError)
    lambda { [mock('not float')].pack(format) }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns a ASCII-8BIT string" do
      [-1.0].pack(format).encoding.should == Encoding::ASCII_8BIT
    end
  end
end

describe "Array#pack with float format (IEEE754 single precision, big endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string with length 4" do
    [1.0].pack(format).length.should == 4
  end

  it "accepts the positive zero" do
    [+0.0].pack(format).should == encode("\x00\x00\x00\x00", "binary")
  end
  it "accepts the negative zero" do
    [-0.0].pack(format).should == encode("\x80\x00\x00\x00", "binary")
  end
  it "accepts a positive value" do
    [+1.0].pack(format).should == encode("\x3F\x80\x00\x00", "binary")
  end
  it "accepts a negative value" do
    [-1.0].pack(format).should == encode("\xBF\x80\x00\x00", "binary")
  end
  it "accepts the positive infinity" do
    [1.0/0.0].pack(format).should == encode("\x7F\x80\x00\x00", "binary")
  end
  it "accepts the negative infinity" do
    [-1.0/0.0].pack(format).should == encode("\xFF\x80\x00\x00", "binary")
  end
  it "accepts a NaN" do
    [0.0/0.0].pack(format).should =~ /[\x7F\xFF](?:[\x81-\xFF]..|\x80[\x01-\xFF].|\x80.[\x01-\xFF])/n
  end

  it "keeps order of nonnegative real numbers" do
    numbers = [ 0.0, 1.0e-126, 1.5e-126, 0.5, 1.0, 1.5, 1.0e127, 1.5e127, 1.0/0.0 ]
    packed = numbers.map{|f| [f].pack(format) }
    packed.should == packed.sort
  end

  it "reverses order of nonpositive real numbers" do
    numbers = [ -1.0/0.0, -1.5e127, -1.0e127, -1.5, -1.0, -0.5, -1.5e-126, -1.0e-126, -0.0 ]
    packed = numbers.map{|f| [f].pack(format) }
    packed.should == packed.sort.reverse
  end
end

describe "Array#pack with float format (IEEE754 single precision, little endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string with length 4" do
    [1.0].pack(format).length.should == 4
  end

  it "accepts the positive zero" do
    [+0.0].pack(format).should == encode("\x00\x00\x00\x00", "binary")
  end
  it "accepts the negative zero" do
    [-0.0].pack(format).should == encode("\x00\x00\x00\x80", "binary")
  end
  it "accepts a positive value" do
    [+1.0].pack(format).should == encode("\x00\x00\x80\x3F", "binary")
  end
  it "accepts a negative value" do
    [-1.0].pack(format).should == encode("\x00\x00\x80\xBF", "binary")
  end
  it "accepts the positive infinity" do
    [1.0/0.0].pack(format).should == encode("\x00\x00\x80\x7F", "binary")
  end
  it "accepts the negative infinity" do
    [-1.0/0.0].pack(format).should == encode("\x00\x00\x80\xFF", "binary")
  end
  it "accepts a NaN" do
    [0.0/0.0].pack(format).should =~ /(?:..[\x81-\xFF]|.[\x01-\xFF]\x80|[\x01-\xFF].\x80)[\x7F\xFF]/n
  end
end

describe "Array#pack with float format (IEEE754 double precision, big endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string with length 8" do
    [1.0].pack(format).length.should == 8
  end

  it "accepts the positive zero" do
    [+0.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts the negative zero" do
    [-0.0].pack(format).should == encode("\x80\x00\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts a positive value" do
    [+1.0].pack(format).should == encode("\x3F\xF0\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts a negative value" do
    [-1.0].pack(format).should == encode("\xBF\xF0\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts the positive infinity" do
    [1.0/0.0].pack(format).should == encode("\x7F\xF0\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts the negative infinity" do
    [-1.0/0.0].pack(format).should == encode("\xFF\xF0\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts a NaN" do
    [0.0/0.0].pack(format).should =~ /[\x7F\xFF](?:[\xF1-\xFF].{6}|\xF0\x00*[\x01-\xFF]\x00*)/n
  end

  it "keeps order of nonnegative real numbers" do
    numbers = [ 0.0, 1.0e-1022, 1.5e-1022, 0.5, 1.0, 1.5, 1.0e1023, 1.5e1023, 1.0/0.0 ]
    packed = numbers.map{|f| [f].pack(format) }
    packed.should == packed.sort
  end

  it "reverses order of nonpositive real numbers" do
    numbers = [ -1.0/0.0, -1.5e1023, -1.0e1023, -1.5, -1.0, -0.5, -1.5e-1022, -1.0e-1022, -0.0 ]
    packed = numbers.map{|f| [f].pack(format) }
    packed.should == packed.sort.reverse
  end
end

describe "Array#pack with float format (IEEE754 double precision, little endian)", :shared => true do
  before do
    @format = @method
    def self.format(count = nil)
      "#{@format}#{count}"
    end
  end

  it "returns a string with length 8" do
    [1.0].pack(format).length.should == 8
  end

  it "accepts the positive zero" do
    [+0.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x00", "binary")
  end
  it "accepts the negative zero" do
    [-0.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\x00\x80", "binary")
  end
  it "accepts a positive value" do
    [+1.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\xF0\x3F", "binary")
  end
  it "accepts a negative value" do
    [-1.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\xF0\xBF", "binary")
  end
  it "accepts the positive infinity" do
    [1.0/0.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\xF0\x7F", "binary")
  end
  it "accepts the negative infinity" do
    [-1.0/0.0].pack(format).should == encode("\x00\x00\x00\x00\x00\x00\xF0\xFF", "binary")
  end
  it "accepts a NaN" do
    [0.0/0.0].pack(format).should =~ /(?:.{6}[\xF1-\xFF]|\x00*[\x01-\xFF]\x00*)[\x7F\xFF]/n
  end
end

describe "Array#pack with format 'f'" do
  it_behaves_like "Array#pack with float format", 'f'

  # TODO: non IEEE754 platform
  little_endian { it_behaves_like "Array#pack with float format (IEEE754 single precision, little endian)", 'f' }
  big_endian { it_behaves_like "Array#pack with float format (IEEE754 single precision, big endian)", 'f' }
end

describe "Array#pack with format 'd'" do
  it_behaves_like "Array#pack with float format", 'd'

  # TODO: non IEEE754 platform
  little_endian { it_behaves_like "Array#pack with float format (IEEE754 double precision, little endian)", 'd' }
  big_endian { it_behaves_like "Array#pack with float format (IEEE754 double precision, big endian)", 'd' }
end

describe "Array#pack with format 'e'" do
  it_behaves_like "Array#pack with float format", 'e'

  # TODO: non IEEE754 platform
  it_behaves_like "Array#pack with float format (IEEE754 single precision, little endian)", 'e'
end

describe "Array#pack with format 'E'" do
  it_behaves_like "Array#pack with float format", 'E'

  # TODO: non IEEE754 platform
  it_behaves_like "Array#pack with float format (IEEE754 double precision, little endian)", 'E'
end

describe "Array#pack with format 'g'" do
  it_behaves_like "Array#pack with float format", 'g'

  # TODO: non IEEE754 platform
  it_behaves_like "Array#pack with float format (IEEE754 single precision, big endian)", 'g'
end

describe "Array#pack with format 'G'" do
  it_behaves_like "Array#pack with float format", 'G'

  # TODO: non IEEE754 platform
  it_behaves_like "Array#pack with float format (IEEE754 double precision, big endian)", 'G'
end






describe "Array#pack with format 'M'" do
  it "encodes string with Quoted Printable encoding" do
    ["ABCDEF"].pack('M').should == "ABCDEF=\n"
  end

  it "doesn't encode new line chars" do
    ["\nA"].pack('M').should == "\nA=\n"
  end

  it "always appends soft line break at the end of encoded string" do
    ["ABC"].pack('M')[-2, 2].should == "=\n"
  end

  it "appends soft line break after each 72 chars + 1 encoded char in encoded string by default" do
    s = ["A"*150].pack('M')
    s.should ==
      "A"*73 + "=\n" +
      "A"*73 + "=\n" +
      "A"* 4 + "=\n"

    s = ["A"*72+"\001"].pack('M')
    s.should == "A"*72 + "=01=\n"
  end

  it "appends soft line break after each specified number of chars + 1 encoded char in encoded string for the specified number > 1" do
    s = ["A"*12].pack('M4')
    s.should == "AAAAA=\nAAAAA=\nAA=\n"

    s = ["A"*12+"\001"].pack('M4')
    s.should == "AAAAA=\nAAAAA=\nAA=01=\n"

    s = ["A"*14+"\001"].pack('M4')
    s.should == "AAAAA=\nAAAAA=\nAAAA=01=\n"

    s = ["A"*15+"\001"].pack('M4')
    s.should == "AAAAA=\nAAAAA=\nAAAAA=\n=01=\n"
  end

  it "appends soft line break after each 72 chars + 1 encoded char in encoded string for the specified count is 1" do
    s = ["A"*150].pack('M1')
    s.should ==
      "A"*73 + "=\n" +
      "A"*73 + "=\n" +
      "A"* 4 + "=\n"

    s = ["A"*72+"\001"].pack('M1')
    s.should == "A"*72 + "=01=\n"
  end
  it "appends soft line break after each 72 chars + 1 encoded char in encoded string for the specified count is 0" do
    s = ["A"*150].pack('M0')
    s.should ==
      "A"*73 + "=\n" +
      "A"*73 + "=\n" +
      "A"* 4 + "=\n"

    s = ["A"*72+"\001"].pack('M0')
    s.should == "A"*72 + "=01=\n"
  end

  it "doesn't quote chars 32..60 and 62..126" do
    32.upto(60) do |i|
      [i.chr].pack('M').should == i.chr+"=\n"
    end

    62.upto(126) do |i|
      [i.chr].pack('M').should == i.chr+"=\n"
    end
  end

  it "quotes chars by adding equal sign and char's hex value" do
    ["\001"].pack('M').should == "=01=\n"
  end

  it "quotes equal sign" do
    ["="].pack('M').should == "=3D=\n"
  end

  it "doesn't quote \\t char" do
    ["\t"].pack('M').should == "\t=\n"
  end

  it "returns empty string if source string is empty" do
    [""].pack('M').should == ""
  end

  it "calls #to_s on object to convert to string" do
    obj = mock('to_s')
    obj.should_receive(:to_s).and_return('world')

    [123].pack('M').should == "123=\n"
    [:hello].pack('M').should == "hello=\n"
    [obj].pack('M').should == "world=\n"
  end

  it "does not check whether the pack argument responds to #to_s before call #to_s" do
    obj = mock('to_s')
    obj.should_not_receive(:respond_to?).with(:to_s)
    [obj].pack('M')
  end

  it "comsumres only one array item per a format" do
    ["abc", "def"].pack('M').should == "abc=\n"
    ["abcdef", "ghijkl"].pack('M3M3').should == "abcd=\nef=\nghij=\nkl=\n"
  end

  ruby_bug("[ruby-dev:37289]", "1.8.7.73") do
    it "ignores star parameter" do
      ["X"*150, "not used", "not used", "not used"].pack('M*').should == ["X"*150].pack('M')
    end
  end

  it "properly handles recursive arrays" do
    empty = ArraySpecs.empty_recursive_array
    empty.pack('M').should be_kind_of(String)

    array = ArraySpecs.recursive_array
    array.pack('M').should == "1=\n"
  end

  ruby_version_is '1.9' do
    it "returns an US-ASCII string" do
      ["abcd"].pack('M').encoding.should == Encoding::US_ASCII
    end
  end

  ruby_version_is ""..."1.9" do

    describe "with a multibyte $KCODE" do
      before :each do
        @kcode = $KCODE
      end

      after :each do
        $KCODE = @kcode
      end

      it "encodes multibyte characters" do
        $KCODE = "UTF8"
        ["あ"].pack('M').should == "=E3=81=82=\n"
      end
    end
  end
end

describe "Array#pack with format 'm'" do
  it "encodes string with Base64 encoding" do
    ["ABCDEF"].pack('m').should == "QUJDREVG\n"
  end

  it "converts series of 3-char sequences into four 4-char sequences" do
    ["ABCDEFGHI"].pack('m').size.should == 4+4+4+1
  end

  it "fills chars with non-significant bits with '=' sign" do
    ["A"].pack('m').should == "QQ==\n"
  end

  it "appends newline at the end of result string" do
    ["A"].pack('m')[-1].should == ?\n
  end

  it "appends newline whenever after consumes 45 bytes by default" do
    ["ABC"*31].pack('m').should ==
      "QUJD"*15 + "\n" +
      "QUJD"*15 + "\n" +
      "QUJD\n"
  end

  it "appends newline whenever after consumes [number/3]*3 of bytes for the specified number > 2, where [x] is the floor function" do
    s = ["ABC"*3].pack('m3').should == "QUJD\n"*3
    s = ["ABC"*3+"\x01"].pack('m3').should == "QUJD\n"*3 + "AQ==\n"

    s = ["ABC"*3].pack('m4').should == "QUJD\n"*3
    s = ["ABC"*3+"\x01"].pack('m4').should == "QUJD\n"*3 + "AQ==\n"
    s = ["ABC"*3+"\x01"].pack('m5').should == "QUJD\n"*3 + "AQ==\n"
    s = ["ABC"*3+"\x01"].pack('m6').should == "QUJDQUJD\nQUJDAQ==\n"
  end

  it "ignores line length parameter if it is 1 or 2" do
    wrapped_at_45 =
      "QUJD"*15 + "\n" +
      "QUJD"*15 + "\n" +
      "QUJD\n"
    ["ABC"*31].pack('m1').should == wrapped_at_45
    ["ABC"*31].pack('m2').should == wrapped_at_45
  end


  ruby_version_is '' ... '1.9' do
    it "ignores line length parameter if it is 0" do
      ["ABC"*31].pack('m0').should ==
        "QUJD"*15 + "\n" +
        "QUJD"*15 + "\n" +
        "QUJD\n"
    end
  end
  # feature changed in 1.9 - [ruby-dev:35904].
  ruby_version_is '1.9' do
    it "does not append newline if line length parameter is 0" do
      ["ABC"*31].pack('m0').should == "QUJD"*31
      ["ABC"*31 + "\x01"].pack('m0').should == "QUJD"*31 + "AQ=="
    end
  end

  it "comsumres only one array item per a format" do
    ["ABC", "DEF"].pack('m').should == "QUJD\n"
    ["ABC", "DEF"].pack('m3').should == "QUJD\n"
    ["ABC", "DEF"].pack('m3m3').should == "QUJD\nREVG\n"
  end

  ruby_bug("[ruby-dev:37289]", "1.8.7.73") do
    it "ignores star parameter" do
      ["ABC"*150, 'not used', 'not used', 'not used'].pack('m*').should == ["ABC"*150].pack('m')
      ["ABC"*150, 0x41, 0x42].pack('m*CC').should == ["ABC"*150].pack('m') + "\x41\x42"
    end
  end

  it "encodes 6-bit char less than 26 with capital letters" do
    [( 0*4).chr].pack('m').should == "AA==\n"
    [( 1*4).chr].pack('m').should == "BA==\n"

    [(25*4).chr].pack('m').should == "ZA==\n"
  end

  it "encodes 6-bit char from 26 to 51 with lowercase letters" do
    [(26*4).chr].pack('m').should == "aA==\n"
    [(27*4).chr].pack('m').should == "bA==\n"

    [(51*4).chr].pack('m').should == "zA==\n"
  end

  it "encodes 6-bit char 62 with '+'" do
    [(62*4).chr].pack('m').should == "+A==\n"
  end

  it "encodes 6-bit char 63 with '/'" do
    [(63*4).chr].pack('m').should == "/A==\n"
  end

  it "returns empty string if source string is empty" do
    [""].pack('m').should == ""
  end

  it "tries to convert the pack argument to a String using #to_str" do
    obj = mock('to_str')
    obj.should_receive(:to_str).and_return("ABC")
    [obj].pack('m').should == "QUJD\n"
  end

  it "raises a TypeError if corresponding array item is not string" do
    lambda { [123].pack('m') }.should raise_error(TypeError)
    lambda { [[]].pack('m') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('m') }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns an US-ASCII string" do
      ["abcd"].pack('m').encoding.should == Encoding::US_ASCII
    end
  end
end

describe "Array#pack with format 'U'" do
  it "regards a integer as a Unicode codepoint and encodes into UTF-8 byte sequence" do
    ascii = (0x00 .. 0x7F)
    ascii.each do |cp|
      chr = [cp].pack('U')
      encode(chr, "binary")[0,1].should == cp.chr
    end
    ascii.to_a.pack('U*').should == (0x00 .. 0x7F).map{|c| eval('"\x%02x"' % c)}.join

    codepoints = [
      0x80, 0x7FF, 0x800, 0xFFFF,          # BMP
      0x10000, 0xFFFFF, 0x100000, 0x10FFFF # max of unicode code points
    ]
    codepoints.each do |cp|
      lambda { [cp].pack('U') }.should_not raise_error
    end

    [0x7F, 0x7F].pack('U*').should == "\x7F\x7F"
    [262193, 4736, 191, 12, 107].pack('U*').should == encode("\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B", "utf-8")
    [2**16+1, 2**30].pack('U2').should == encode("\360\220\200\201\375\200\200\200\200\200", "utf-8")
  end

  it "raises an ArgumentError if count is greater than array elements left" do
    lambda { [].pack('U') }.should raise_error(ArgumentError)
    lambda { [1].pack('UU') }.should raise_error(ArgumentError)
  end

  it "raises RangeError when a pack argument is negative" do
    lambda { [-1].pack('U') }.should raise_error(RangeError)
    lambda { [-5].pack('U') }.should raise_error(RangeError)
    lambda { [-2**32].pack('U') }.should raise_error(RangeError)
  end

  it "raises RangeError when a pack argument >= 2**32" do
    lambda { [2**32].pack('U') }.should raise_error(RangeError)
  end

  it "may accept a pack argument > max of Unicode codepoint" do
    lambda { [0x00110000].pack('U') }.should_not raise_error(RangeError) # 22bit
    lambda { [0x04000000].pack('U') }.should_not raise_error(RangeError) # 27bit
    lambda { [0x7FFFFFFF].pack('U') }.should_not raise_error(RangeError) # 31bit
  end

  it "only takes as many elements as specified after ('U')" do
    [0x41, 0x42, 0x43, 0x44].pack('U2').should == "\x41\x42"
    [0x41, 0x42, 0x43, 0x44].pack('U2U').should == "\x41\x42\x43"
  end

  ruby_version_is '1.9' do
    it "returns a UTF-8 string" do
      [0x00].pack('U').encoding.should == Encoding::UTF_8
      [0x41].pack('U').encoding.should == Encoding::UTF_8
      [0x7F].pack('U').encoding.should == Encoding::UTF_8
      [0x80].pack('U').encoding.should == Encoding::UTF_8
      [0x10FFFF].pack('U').encoding.should == Encoding::UTF_8
    end
  end
end

describe "Array#pack with format 'u'" do
  it "encodes string with UU-encoding" do
    ["ABCDEF"].pack('u').should == "&04)#1$5&\n"
  end

  it "converts series of 3-char sequences into four 4-char sequences" do
    ["ABCDEFGHI"].pack('u').size.should == 4+4+4+1+1
  end

  it "appends zero-chars to source string if string length is not multiple of 3" do
    ["A"].pack('u').should == "!00``\n"
  end

  it "appends newline at the end of result string" do
    ["A"].pack('u')[-1].should == ?\n
  end

  it "appends newline whenever after consumes 45 bytes by default" do
    s = ["ABC"*31].pack('u')
    line_len = 1 + 4*(45/3) + 1
    s[1*line_len - 1].should == ?\n
    s[2*line_len - 1].should == ?\n
    s[-1].should == ?\n
  end

  it "prepends encoded line length to each line with ('u')" do
    s = ["ABC"*31].pack('u')
    line_len = 1 + 4*(45/3) + 1
    s[0*line_len, 1].should == (45+0x20).chr
    s[1*line_len, 1].should == (45+0x20).chr
    s[2*line_len, 1].should == (3*31-45*2 + 0x20).chr
  end

  it "appends newline whenever after consumes [number/3]*3 of bytes for the specified number > 2, where [x] is the floor function" do
    s = ["ABC"*3].pack('u3').should == ( (3+0x20).chr + "04)#\n" ) * 3
    s = ["ABC"*3+"\x01"].pack('u3').should ==  ( (3+0x20).chr + "04)#\n" )*3 + (1+0x20).chr + "`0``\n"

    s = ["ABC"*3].pack('u4').should == ( (3+0x20).chr + "04)#\n" ) * 3
    s = ["ABC"*3+"\x01"].pack('u4').should == ( (3+0x20).chr + "04)#\n" )*3 + (1+0x20).chr + "`0``\n"
    s = ["ABC"*3+"\x01"].pack('u5').should == ( (3+0x20).chr + "04)#\n" )*3 + (1+0x20).chr + "`0``\n"
    s = ["ABC"*3+"\x01"].pack('u6').should ==
      (6+0x20).chr + "04)#04)#\n" +
      (4+0x20).chr + "04)#`0``\n"
  end

  it "ignores line length parameter if it is < 3" do
    wrapped_at_45 =
        (45+0x20).chr + "04)#"*(45/3) + "\n" +
        (45+0x20).chr + "04)#"*(45/3) + "\n" +
        ( 3+0x20).chr + "04)#" + "\n"

    ["ABC"*31].pack('u0').should == wrapped_at_45
    ["ABC"*31].pack('u1').should == wrapped_at_45
    ["ABC"*31].pack('u2').should == wrapped_at_45
  end

  it "comsumres only one array item per a format" do
    ["ABC", "DEF"].pack('u').should == "#04)#\n"
    ["ABC", "DEF"].pack('u3').should == "#04)#\n"
    ["ABC", "DEF"].pack('u3u3').should == "#04)#\n#1$5&\n"
  end

  it "ignores star parameter" do
    ["ABC"*150, 'not used', 'not used', 'not used'].pack('u*').should == ["ABC"*150].pack('u')
    ["ABC"*150, 0x41, 0x42].pack('u*CC').should == ["ABC"*150].pack('u') + "\x41\x42"
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

  it "tries to convert the pack argument to a String using #to_str" do
    obj = mock('to_str')
    obj.should_receive(:to_str).and_return("ABC")
    [obj].pack('u').should == "#04)#\n"
  end

  it "raises a TypeError if corresponding array item is not string" do
    lambda { [123].pack('u') }.should raise_error(TypeError)
    lambda { [[]].pack('u') }.should raise_error(TypeError)
    lambda { [mock('not string')].pack('u') }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns an US-ASCII string" do
      ["abcd"].pack('u').encoding.should == Encoding::US_ASCII
    end
  end
end

describe "Array#pack with format 'w'" do
  it "converts to BER-compressed integer" do
    [0].pack('w').should == encode("\000", "binary")
    [1].pack('w').should == encode("\001", "binary")
    [0, 1, 2].pack('w2').should == encode("\000\001", "binary")
    [0, 1, 2].pack('w*').should == encode("\000\001\002", "binary")
    [9999].pack('w').should == encode("\316\017", "binary")
    [2**64].pack('w').should == encode("\202\200\200\200\200\200\200\200\200\000", "binary")
  end

  it "raises ArgumentError when a pack argument is negative" do
    lambda { [-1].pack('w') }.should raise_error(ArgumentError)
    lambda { [-2**256].pack('w') }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if the count is greater than the number of remaining array elements" do
    lambda { [1].pack('w2') }.should raise_error(ArgumentError)
    lambda { [1, 2, 3, 4, 5].pack('w10') }.should raise_error(ArgumentError)
  end

  it "calls to_int on non-integer values before packing" do
    obj = mock('1')
    obj.should_receive(:to_int).and_return(1)
    [obj].pack('w').should == encode("\001", "binary")
  end

  it "raises TypeError on nil and non-numeric arguments" do
    lambda { [nil].pack('w') }.should raise_error(TypeError)
    lambda { [()].pack('w') }.should raise_error(TypeError)
    lambda { ['a'].pack('w') }.should raise_error(TypeError)
    lambda { [Object.new].pack('w') }.should raise_error(TypeError)
  end

  ruby_version_is '1.9' do
    it "returns an ASCII-8BIT string" do
      [1].pack('w').encoding.should == Encoding::ASCII_8BIT
    end
  end
end


describe "Array#pack with format 'X'" do
  it "decreases result string by one byte" do
    ['abcdef'].pack('A4X').should == 'abc'
  end
  it "with count decreases result string by count bytes" do
    ['abcdef'].pack('A6X4').should == 'ab'
  end

  it "with zero count doesn't change result string" do
    ['abcdef'].pack('A6X0').should == 'abcdef'
  end

  it "treats start parameter as zero count" do
    ['abcdef'].pack('A6X*').should == 'abcdef'
  end

  it "raises an ArgumentError if count greater than already generated string length" do
    lambda { ['abcdef'].pack('A6X7') }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if it is first directive" do
    lambda { [].pack('X') }.should raise_error(ArgumentError)
    lambda { [1].pack('XA') }.should raise_error(ArgumentError)
  end

  it "doesn't increment the array index count" do
    ['abcd','efgh'].pack('A4X2A4').should == 'abefgh'
  end

  ruby_version_is '1.9' do
    it "returns an ASCII 8-bit String" do
      [0x41, 0x42, 0x43].pack('U3X').encoding.should == Encoding::ASCII_8BIT
      [1, 2, 3].pack('w3X').encoding.should == Encoding::ASCII_8BIT
      ["\x01\x02"].pack("mX").encoding.should == Encoding::ASCII_8BIT
    end

    it "doesn't care if it breaks a character" do
      str = nil
      lambda { str = [0x3042].pack("UX") }.should_not raise_error
      str.encoding.should == Encoding::ASCII_8BIT
      str.bytesize.should == 2
    end
  end
end

describe "Array#pack with '@'" do
  it "moves the end of result string into the specified position by offset from head" do
    [0xABCDEF01].pack("N @2").should == encode("\xAB\xCD", "binary")
  end
  it "fills blank with NUL bytes if the position exceeds the end of string" do
    [0xABCDEF01].pack("N @6").should == encode("\xAB\xCD\xEF\x01\x00\x00", "binary")
  end
  it "concatenates successing formats at the position '@' moves it into" do
    [0xABCDEF01, 0x41].pack("N @2 C").should == encode("\xAB\xCD\x41", "binary")
    [0xABCDEF01, 0x41].pack("N @6 C").should == encode("\xAB\xCD\xEF\x01\x00\x00\x41", "binary")
  end
  it "does not recover lost bytes when shorten the string and then extends it again" do
    [0xABCDEF01, 0x41].pack("N @2 @6 C").should == encode("\xAB\xCD\x00\x00\x00\x00\x41", "binary")
  end
  it "is able to work with 'X'" do
    [0xABCDEF01, 0x41].pack("N X @6 C").should == encode("\xAB\xCD\xEF\x00\x00\x00\x41", "binary")
  end

  ruby_version_is '1.9' do
    it "returns a String in ASCII 8-bit" do
      [0x41, 0x42, 0x43].pack('U3@6').encoding.should == Encoding::ASCII_8BIT
      [1, 2, 3].pack('w3@3').encoding.should == Encoding::ASCII_8BIT
      ["\x01\x02"].pack("m@4").encoding.should == Encoding::ASCII_8BIT
    end

    it "doesn't care even if breaks a character" do
      str = nil
      lambda { str = [0x3042].pack("U@2") }.should_not raise_error
      str.encoding.should == Encoding::ASCII_8BIT
      str.bytesize.should == 2
    end
  end
end


describe "Array#pack with format 'x'" do
  it "returns NUL-byte string" do
    [].pack('x').should == "\000"
  end

  it "returns string of count zero chars with count" do
    [].pack('x5').should == "\000\000\000\000\000"
  end

  it "returns empty string with count == 0" do
    [].pack('x0').should == ""
  end

  it "behaves like with count == 0 with star parameter" do
    [].pack('x*').should == ""
  end

  it "doesn't increment the array index count" do
    ['abcd','efgh'].pack('A4x2A4').should == "abcd\000\000efgh"
  end

  ruby_version_is '1.9' do
    it "returns an US-ASCII string" do
      ["abcd"].pack('u').encoding.should == Encoding::US_ASCII
    end
  end
end

describe "Array#pack with format 'P'" do
  it "returns a String who's size is equal to the number of bytes in a machine word" do
    [nil].pack("P").size.should == 1.size
  end
end

describe "String#unpack with 'w' directive" do
  ruby_version_is ""..."1.9" do
    it "produces a BER-compressed integer" do
      [88].pack('w').should == 'X'
      [88,89,90].pack('www').should == 'XYZ'
      [88,89,90].pack('w3').should == 'XYZ'
      [92,48,48,49].pack('w4').should == '\001'
      [104,101,108,108,111,32,119,111,114,108,100].pack('w*').should == 'hello world'
      [1234567890].pack('w').should == "\204\314\330\205R"
    end
  end

  ruby_version_is "1.9" do
    it "produces a BER-compressed integer" do
      [88].pack('w').should == 'X'
      [88,89,90].pack('www').should == 'XYZ'
      [88,89,90].pack('w3').should == 'XYZ'
      [92,48,48,49].pack('w4').should == '\001'
      [104,101,108,108,111,32,119,111,114,108,100].pack('w*').should == 'hello world'
      [1234567890].pack('w').should == "\204\314\330\205R".force_encoding(Encoding::ASCII_8BIT)
    end
  end
end

# vim:fileencoding=iso-8859-1
