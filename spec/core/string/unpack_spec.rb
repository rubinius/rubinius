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

describe "String#unpack with Z pattern" do
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
    "\xF3\x02".unpack('Q').should == [nil]
    "\xF3\xFF\xFF\xFF\x32\x0B\x02\x00".unpack('Q2').should == [575263624658931, nil]
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
