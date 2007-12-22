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
    "\xF3\x02\x00\x42\x3A\x87\xF3\x00".unpack('N*').should == [4076994626, 981988096]
    "\xF3\x02\x00\x42".unpack('n').should == [62210]
    "\xF1\x02\x00\x42".unpack('n-7').should == [61698]
    "\xF3\x02\x00\x42".unpack('n5').should == [62210, 66, nil, nil, nil]
    "\xF3".unpack('n').should == [nil]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n3').should == [62210, 66, 12935]
    "\xF3\x02\x00\x42\x32\x87\xF3\x00".unpack('n*').should == [62210, 66, 12935, 62208]
    "\xF3\x02\x00\x42\x32\x87\xF3\x02".unpack('n0N*').should == [4076994626, 847770370]
  end
end
