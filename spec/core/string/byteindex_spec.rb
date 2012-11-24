# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)

describe "String#byteindex with Fixnum" do
  it "returns the byte index of a character index" do
    "abcde".byteindex(1).should == 1
  end

  it "raises an ArgumentError if the index is negative" do
    lambda { "abc".byteindex(-1) }.should raise_error(ArgumentError)
  end

  with_feature :encoding do
    it "returns the byte index of a multibyte character index" do
      "あそこ".byteindex(1).should == 3
    end
  end
end

describe "String#byteindex with String" do
  it "returns the byte index of a character" do
    "abcde".byteindex("e").should == 4
  end

  it "returns nil if the character is not found" do
    "abc".byteindex("e").should be_nil
  end

  it "returns 0 if the search String is empty" do
    "abc".byteindex("").should == 0
  end

  with_feature :encoding do
    it "returns the byte index of a multibyte character" do
      "あそこ".byteindex("こ").should == 6
    end

    it "returns nil if the search String is not a character boundary" do
      "あそこ".byteindex("\x82").should be_nil
    end

    it "returns nil if the search String is not a complete character" do
      "あそこ".byteindex("\xe3\x82").should be_nil
    end

    it "raises an ArgumentError if the encoding of String and pattern are incompatible" do
      pattern = "こ".encode Encoding::EUC_JP
      lambda { "あそこ".byteindex pattern }.should raise_error(ArgumentError)
    end
  end
end
