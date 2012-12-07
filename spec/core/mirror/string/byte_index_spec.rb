# -*- encoding: utf-8 -*-
require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "Rubinius::Mirror::String#byte_index with Fixnum" do
  it "returns 0 for index 0 of an empty String" do
    string_mirror("").byte_index(0).should == 0
  end

  it "returns the byte index of a character index" do
    string_mirror("abcde").byte_index(1).should == 1
  end

  it "raises an ArgumentError if the index is negative" do
    lambda { string_mirror("abc").byte_index(-1) }.should raise_error(ArgumentError)
  end

  with_feature :encoding do
    it "returns the byte index of a multibyte character index" do
      string_mirror("あそこ").byte_index(1).should == 3
    end
  end
end

describe "String#byte_index with String" do
  it "returns the byte index of a character" do
    string_mirror("abcde").byte_index("e").should == 4
  end

  it "returns nil if the character is not found" do
    string_mirror("abc").byte_index("e").should be_nil
  end

  it "returns 0 if the search String is empty" do
    string_mirror("abc").byte_index("").should == 0
  end

  with_feature :encoding do
    it "returns the byte index of a multibyte character" do
      string_mirror("あそこ").byte_index("こ").should == 6
    end

    it "returns nil if the search String is not a character boundary" do
      string_mirror("あそこ").byte_index("\x82").should be_nil
    end

    it "returns nil if the search String is not a complete character" do
      string_mirror("あそこ").byte_index("\xe3\x82").should be_nil
    end

    it "raises an ArgumentError if the encoding of String and pattern are incompatible" do
      pattern = "こ".encode Encoding::EUC_JP
      lambda do
        string_mirror("あそこ").byte_index pattern
      end.should raise_error(ArgumentError)
    end
  end
end
