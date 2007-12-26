require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#unpack" do
  it "returns an array by decoding self according to the format string" do
    "abc \0\0abc \0\0".unpack('A6Z6').should == ["abc", "abc "]
    "abc \0\0".unpack('a3a3').should == ["abc", " \000\000"]
    "aa".unpack('b8B8').should == ["10000110", "01100001"]
    "aaa".unpack('h2H2c').should == ["16", "61", 97]
    compliant (:ruby) do
      # Note: The result depends on the platform the test
      # is being executed (not good).
      # Also, For JRuby, the byte order is always big-endian.
      "\xfe\xff\xfe\xff".unpack('sS').should == [-2, 65534]
    end
    "now=20is".unpack('M*').should == ["now is"]
    "whole".unpack('xax2aX2aX1aX2a').should == ["h", "e", "l", "l", "o"]
  end
end

describe "String#unpack with nil format argument" do
  it "raises a TypeError exception" do
    lambda { "abc".unpack(nil) }.should raise_error(TypeError)
  end
end

describe "String#unpack with 'Z' directive" do
  it "returns an array by decoding self according to the format string" do
    "abc \0abc \0".unpack('Z*Z*').should == ["abc ", "abc "]
    "abc \0abc \0".unpack('Z10').should == ["abc "]
    "abc \0abc \0".unpack('Z7Z*').should == ["abc ", "c "]
    "abc \0abc \0".unpack('Z50Z*').should == ["abc ", ""]
    "abc \0\0\0abc \0".unpack('Z*Z*').should == ["abc ", ""]
    "abc \0\0\0\0".unpack('Z*').should == ["abc "]
    "abc \0\0\0\0".unpack('Z*Z*').should == ["abc ", ""]
    "\0".unpack('Z*').should == [""]
    "\0\0".unpack('Z*').should == [""]
    "\0\0abc".unpack('Z*').should == [""]
    "\0\0abc\0\0".unpack('Z*').should == [""]
    "\0 abc\0\0abc\0abc \0\0 ".unpack('Z2Z*Z3ZZ*Z10Z10').should == ["", "abc", "", "c", "", "abc ", ""]
  end
end

describe "String#unpack with 'N' and 'n' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('N').should == [4076994626]
    "\xF3\x02".unpack('N').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('N2').should == [4076994626, 847770368]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('N*').should == [4077043778, 981988096]
    "\xF3\x02\x00\x42".unpack('n').should == [62210]
    "\x01\x62\xEE\x42".unpack('n-7n').should == [354, 60994]
    "\xF3\x02\x00\x42".unpack('n5').should == [62210, 66, nil, nil, nil]
    "\xF3".unpack('n').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n3').should == [62210, 66, 12935]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n*').should == [62210, 66, 12935, 62208]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('n0N*').should == [4076994626, 847770370]
  end
end

describe "String#unpack with 'V' and 'v' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('V').should == [1107297011]
    "\xF3\x02".unpack('V').should == [nil]
    "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('V2').should == [4294967283, 133938]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('V*').should == [1119879923, 15959866]
    "\xF3\x02\x00\x42".unpack('v').should == [755]
    "\x01\x62\xEE\x42".unpack('v-7v').should == [25089, 17134]
    "\xF3\x02\x00\x42".unpack('v5').should == [755, 16896, nil, nil, nil]
    "\xF3".unpack('v').should == [nil]
    "\xF3\xFF\xFF\xFF\x32\x87\xF3\x00".unpack('v3').should == [65523, 65535, 34610]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('v*').should == [755, 16896, 34610, 243]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('v0V*').should == [1107297011, 49514290]
  end
end

describe "String#unpack with 'C' and 'c' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42".unpack('C').should == [243]
    "".unpack('C').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('C2').should == [243, 2]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('C*').should == [243, 2, 192, 66, 58, 135, 243, 0]
    "\xF3\x02\x00\x42".unpack('c').should == [-13]
    "\x01\x62\xEE\x42".unpack('c-7c').should == [1, 98]
    "\xF3\x02\x00\x42".unpack('c5').should == [-13, 2, 0, 66, nil]
    "\x80".unpack('c').should == [-128]
    "\x7F\x02\x00\x42\x32\x87\xF3\x00".unpack('c3').should == [127, 2, 0]
    "\x80\x02\x00\x42\xFF\x87\xF3\x00".unpack('c*').should == [-128, 2, 0, 66, -1, -121, -13, 0]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('c0C*').should == [243, 2, 0, 66, 50, 135, 243, 2]
  end
end

describe "String#unpack with 'Q' and 'q' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('Q').should == [17344245288696546035]
    "\xF3\x02".unpack('Q*').should == []
    "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('Q2').should == [575263624658931]
    "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('Q*').should ==
      [68547103638422259, 17218254449219272698]
    "\xF3\x02\x00\x42".unpack('q').should == [nil]
    "\x01\x62\xEE\x42".unpack('q-7q').should == [nil, nil]
    "\xF3\x02\x00\x42".unpack('q5').should == [nil, nil, nil, nil, nil]
    "\xF3".unpack('q').should == [nil]
    "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('q3').should == [-147498385354522637, nil, nil]
    "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('q*').should ==
      [68547068192358922, -1228489624490278918]
    "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('q0Q*').should == [8608480567731124095]
  end
end

describe "String#unpack with 'a', 'X' and 'x' directives" do
  it "returns an array by decoding self according to the format string" do
    "abc".unpack('x3a').should == [""]
    "abc".unpack('x3X3x0a').should == ["a"]
    "abc".unpack('x2X-5a').should == ["b"]
    "abcdefghijklmnopqrstuvwxyz".unpack('aaax10aX14X0a').should == ["a", "b", "c", "n", "a"]
    "abcdefghijklmnopqrstuvwxyz".unpack('aa-8ax10aX*a').should == ["a", "b", "c", "n", "c"]
    "abcdefghijklmnopqrstuvwxyz".unpack('x*aX26a').should == ["", "a"]
    "abcdefghijklmnopqrstuvwxyz".unpack('x*x*x*aX26a').should == ["", "a"]
    "abc".unpack('x3X*').should == []
    "abc".unpack('x3X*a').should == [""]
    "abc".unpack('x3X*X3a').should == ["a"]
    lambda { "abc".unpack('X*') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('x*x') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('x4') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('X') }.should raise_error(ArgumentError)
    lambda { "abc".unpack('xX*') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'DdEeFfGg' directives" do
  it "returns an array by decoding self according to the format string" do
    precision_small = 1E-12
    precision_large = 1E+17

    res = "\xF3\x02\x00\x42\xF3\x02\x00\x42".unpack('eg')
    res.length.should == 2
    res[0].should be_close(32.0028800964355, precision_small)
    res[1].should be_close(-1.02997409159585e+31, precision_large)

    res = "\xF3\x02\x00\x42\xF3\x02\x00\x42".unpack('eg')
    res.length.should == 2
    res[0].should be_close(32.0028800964355, precision_small)
    res[1].should be_close(-1.02997409159585e+31, precision_large)

    "\xF3\x02".unpack('GD').should == [nil, nil]

    # 'F2' pattern
    res = "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('F2')
    compliant :jruby do
      # In JRuby, the "native" byte order is always big-endian.
      res = "\xFF\xFF\xFF\xF3\x00\x02\x0B\x32".unpack('F2')
    end
    res.length.should == 2
    res[0].nan?.should == true
    res[1].should be_close(1.87687113714737e-40, precision_small)

    # 'f*' pattern
    res = "\xF3\x02\xC0\x42\x3A\x87\xF3\x00".unpack('f*')
    compliant :jruby do
      res = "\x42\xC0\x02\xF3\x00\xF3\x87\x3A".unpack('f*')
    end
    res.length.should == 2
    res[0].should be_close(96.0057601928711, precision_small)
    res[1].should be_close(2.23645357166299e-38, precision_small)

    "\xFF\x80\x00\x00".unpack('g').to_s.should == "-Infinity"
    "\x01\x62\xEE\x42".unpack('e-7e')[0].should  be_close(
        119.191413879395, precision_small)
    "\x00\x00\x00\x00".unpack('f5').should == [0.0, nil, nil, nil, nil]
    "\xF3".unpack('E').should == [nil]

    # 'd3' pattern
    res = "\xF3\xFF\xFF\xFF\x32\x87\xF3\x00".unpack('d3')
    compliant :jruby do
      # In JRuby, the "native" byte order is always big-endian.
      res = "\x00\xF3\x87\x32\xFF\xFF\xFF\xF3".unpack('d3')
    end
    res.length.should == 3
    res[0].should be_close(4.44943499804409e-304, precision_small)
    res[1].should == nil
    res[2].should == nil

    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('E*')[0].should be_close(
        4.44943241769783e-304, precision_small)
    "\x00\x00\x00\x42\x32\x87\xF3\x02".unpack('g0G*')[0].should be_close(
        1.40470576419087e-312, precision_small)
  end
end

describe "String#unpack with 'B' and 'b' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B64').should ==
      ["1111111100000000100000000000000101111110101010101111000000001111"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('b64').should ==
      ["1111111100000000000000011000000001111110010101010000111111110000"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B12b12B12b12B*b*').should ==
      ["111111110000", "000000011000", "011111101010", "000011111111", "", ""]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('B4b4B4b4B4b4B16').should ==
      ["1111", "0000", "1000", "1000", "0111", "0101", "1111000000001111"]
    "\xFF\x00\x80\xAA".unpack('b-5B8b8b0B2b1B1').should ==
      ["1", "00000000", "00000001", "", "10", "", ""]
    "\xFF\x00\x80\xAA".unpack('B0b0').should == ["", ""]
    "\xFF\x00\x80\xAA".unpack('B3b*B*').should == ["111", "000000000000000101010101", ""]
    "\xFF\x00\x80\xAA".unpack('B3B*b*').should == ["111", "000000001000000010101010", ""]
  end
end

describe "String#unpack with 'H' and 'h' directives" do
  it "returns an array by decoding self according to the format string" do
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('H16').should == ["ff0080017eaaf00f"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('h16').should == ["ff000810e7aa0ff0"]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('H3h3H3h3H*h*').should ==
      ["ff0", "081", "7ea", "0ff", "", ""]
    "\xFF\x00\x80\x01\x7E\xAA\xF0\x0F".unpack('HhHhHhH4').should ==
      ["f", "0", "8", "1", "7", "a", "f00f"]
    "\xFF\x00\x80\xAA".unpack('h-5H2h2h0Hh1H1').should ==
      ["f", "00", "08", "", "a", "", ""]
    "\xFF\x00\x80\xAA".unpack('H0h0').should == ["", ""]
    "\xFF\x00\x80\xAA".unpack('H3h*H*').should == ["ff0", "08aa", ""]
    "\xFF\x00\x80\xAA".unpack('H3H*h*').should == ["ff0", "80aa", ""]
  end
end

describe "String#unpack with 'IiLlSs' directives" do
  platform :size => 32 do
    it "returns an array by decoding self according to the format string" do
      "\xF3\x02\x00\x42\x32\x23\xB3\xF0".unpack('SLI').should == [755, 590496256, nil]
      "\xF3\x02".unpack('L*I*').should == []
      "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('I2').should == [4294967283, 133938]
      "\xF3\x02\xC0\x42\x3A\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('L*').should ==
        [1119879923, 15959866, 33619962, 4008937266]
      "\xF3\x02\x00\x42".unpack('i').should == [1107297011]
      "\x01\x62\xEE\x42".unpack('l-7l').should == [1122918913, nil]
      "\xF3\x02\x00\x42".unpack('s5').should == [755, 16896, nil, nil, nil]
      "\xF3".unpack('s').should == [nil]
      "\xF3\xFF\xFF\xFF\xFF\xFA\xF3\xFD".unpack('i3').should == [-13, -34342145, nil]
      "\x0A\x02\x00\x02\x32\x87\xF3\x00\xFA\xFF\x00\x02\x32\x87\xF3\xEE".unpack('l*').should ==
        [33554954, 15959858, 33619962, -286030030]
      "\x7F\x77\x77\x77\x77\x77\x77\x77".unpack('i0I*').should == [2004318079, 2004318071]
    end
  end
end

describe "String#unpack with 'U' directive" do
  it "returns an array by decoding self according to the format string" do
    "\xFD\x80\x80\xB7\x80\x80".unpack('U').should == [1073967104]
    "\xF9\x80\x80\x80\x80".unpack('U').should == [16777216]
    "\xF1\x80\x80\x80".unpack('UU').should == [262144]
    "\xE1\xB7\x80".unpack('U').should == [7616]
    "\xC2\x80\xD2\x80".unpack('U-8U').should == [128, 1152]
    "\x00\x7F".unpack('U100').should == [0, 127]
    "\x05\x7D".unpack('U0U0').should == []
    "".unpack('U').should == []
    "\xF1\x80\x80\xB1\xE1\x8A\x80\xC2\xBF\x0C\x6B".unpack('U*').should == [262193, 4736, 191, 12, 107]
    "\xF1\x8F\x85\xB1\xE1\x8A\x89\xC2\xBF\x0C\x6B".unpack('U2x2U').should == [323953, 4745, 12]
    lambda { "\xF0\x80\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE0\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC0\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC1\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xF1\x80\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE1\x80".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC2".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xF1\x00\x00\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xE1\x00\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xC2\x00".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\xFE".unpack('U') }.should raise_error(ArgumentError)
    lambda { "\x03\xFF".unpack('UU') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'A' directive" do
  it "returns an array by decoding self according to the format string" do
    "".unpack('A').should == [""]
    " \0 abc \0\0\0abc\0\0 \0abc".unpack('A*').should ==
      [" \000 abc \000\000\000abc\000\000 \000abc"]
    " \0 abc \0\0\0abc\0\0 \0abc \0 a ".unpack('A16A-9A*A*A100').should ==
      [" \000 abc \000\000\000abc", "", "abc \000 a", "", ""]
    " \0 abc \0\0\0abc\0\0 \0abc \0 a ".unpack('A3A4AAA').should == ["", "abc", "", "", ""]
    " \0 abc \0\0\0abc\0\0 \0abc \0 a ".unpack('A2A0A14').should == ["", "", " abc \000\000\000abc"]
  end
end

describe "String#unpack with '@' directive" do
  it "returns an array by decoding self according to the format string" do
    "abcdefg".unpack('@2').should == []
    "abcdefg".unpack('@3@-5a').should == ["a"]
    "abcdefg".unpack('@*@a').should == ["a"]
    "abcdefg".unpack('@3@5a').should == ["f"]
    "abcdefg".unpack('@*a').should == [""]
    "abcdefg".unpack('@7a').should == [""]
    lambda { "abcdefg".unpack('@8') }.should raise_error(ArgumentError)
  end
end

describe "String#unpack with 'M' directive" do
  it "returns an array by decoding self according to the format string" do
    "".unpack('M').should == [""]
    "=5".unpack('Ma').should == ["", ""]
    "abc=".unpack('M').should == ["abc"]
    "a=*".unpack('MMMM').should == ["a", "*", "", ""]
    "\x3D72=65abcdefg=5%=33".unpack('M').should == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M0').should == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M100').should == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M*').should == ["reabcdefg"]
    "\x3D72=65abcdefg=5%=33".unpack('M-8M').should == ["reabcdefg", "%3"]
    "abc===abc".unpack('MMMMM').should == ["abc", "", "\253c", "", ""]
    "=$$=47".unpack('MMMM').should == ["", "$$G", "", ""]
    "=$$=4@=47".unpack('MMMMM').should == ["", "$$", "@G", "", ""]
    "=$$=4@=47".unpack('M5000').should == [""]
    "=4@".unpack('MMM').should == ["", "@", ""]
  end
end
