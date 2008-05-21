require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/initialize_exceptions'
require 'iconv'

# These specs assume the Iconv implementation supports at least
# the following encodings:
#   us-ascii, utf-8, utf-16, utf-16be, utf-16le, iso-8859-1

describe "Iconv#iconv" do
  it "raises an ArgumentError when called on a closed converter" do
    conv = Iconv.new("us-ascii", "us-ascii")
    conv.close
    lambda { conv.iconv("test") }.should raise_error(ArgumentError)
  end

  it "when given a string or string-like parameter returns a converted version of it" do
    Iconv.open "utf-8", "iso-8859-1" do |conv|
      conv.iconv("expos\xe9").should == "expos\xc3\xa9"

      stringlike = mock("string-like")
      stringlike.should_receive(:to_str).and_return("r\xe9sum\xe9")
      conv.iconv(stringlike).should == "r\xc3\xa9sum\xc3\xa9"
    end
  end

  it "keeps context between calls" do
    Iconv.open "utf-16", "us-ascii" do |conv|
      # BOM for first call of utf-16
      conv.iconv("a").should equal_utf16("\xfe\xff\0a")
      # no BOM for consecutive calls
      conv.iconv("a").should equal_utf16("\0a")
    end
  end

  it "when given nil resets the converter" do
    Iconv.open "utf-16", "utf-8" do |conv|
      conv.iconv("a").should equal_utf16("\xfe\xff\0a")
      conv.iconv("a").should equal_utf16("\0a")
      conv.iconv(nil)
      conv.iconv("a").should equal_utf16("\xfe\xff\0a")
    end
  end

  it "when given a negative start position counts from the end of string" do
    Iconv.open "us-ascii", "us-ascii" do |conv|
      conv.iconv("testing", -7, 4).should == "test"
      conv.iconv("testing", -3, 7).should == "ing"
    end
  end

  it "when the end parameter is omitted or nil goes until the end of the string" do
    Iconv.open "us-ascii", "us-ascii" do |conv|
      conv.iconv("testing", 0).should == "testing"
      conv.iconv("testing", 4).should == "ing"
      conv.iconv("testing", 4, nil).should == "ing"
      conv.iconv("testing", -3).should == "ing"
      conv.iconv("testing", -4, nil).should == "ting"
    end
  end

  it "when given a positive end position treats it as exclusive" do
    # i.e. string[start...end]
    Iconv.open "us-ascii", "us-ascii" do |conv|
      conv.iconv("testing", 0, 4).should == "test"
      conv.iconv("testing", 4, 6).should == "in"
      conv.iconv("substring", -6, 6).should == "str"
    end
  end

  it "when given a negative end position treats it as inclusive" do
    # i.e. string[start..end]
    Iconv.open "us-ascii", "us-ascii" do |conv|
      conv.iconv("testing", 0, -1).should == "testing"
      conv.iconv("testing", 2, -4).should == "st"
      conv.iconv("substring", -6, -4).should == "str"
    end
  end

  it "to utf-16, when given an empty string or empty range, always returns an empty string" do
    Iconv.open "utf-16", "utf-8" do |conv|
      conv.iconv("").should == "" # no BOM
      conv.iconv("test", 0, 0).should == ""
      conv.iconv("test", 2, -3).should == ""
      conv.iconv("test", 3, 2).should == ""
      conv.iconv("test", 1, -4).should == ""
    end
  end

  it "raises Iconv::IllegalSequence when faced with an invalid byte" do
    Iconv.open "us-ascii", "us-ascii" do |conv|
      lambda { conv.iconv("test\xa0") }.should raise_error(Iconv::IllegalSequence)
    end

    Iconv.open "utf-8", "utf-8" do |conv|
      lambda { conv.iconv("test\x80") }.should raise_error(Iconv::IllegalSequence)
    end
  end

  it "raises Iconv::IllegalSequence when a character cannot be represented on the target encoding" do
    Iconv.open "us-ascii", "utf-8" do |conv|
      lambda { conv.iconv("euro \xe2\x82\xac") }.should raise_error(Iconv::IllegalSequence)
    end
  end

  it "raises Iconv::InvalidCharacter when an incomplete character or shift sequence happens at the end of the input buffer" do
    Iconv.open "utf-8", "utf-8" do |conv|
      lambda { conv.iconv("euro \xe2") }.should raise_error(Iconv::InvalidCharacter)
      lambda { conv.iconv("euro \xe2\x82") }.should raise_error(Iconv::InvalidCharacter)
    end
    Iconv.open "utf-16be", "utf-16be" do |conv|
      lambda { conv.iconv("a") }.should raise_error(Iconv::InvalidCharacter)
    end
  end

  it "ignores characters which cannot be represented in the target encoding when the //ignore option is set for the target encoding" do
    Iconv.open "iso-8859-1//ignore", "utf-8" do |conv|
      conv.iconv("euro \xe2\x82\xac euro").should == "euro  euro"
    end
  end

  platform_is :os => :linux do
    it "transliterates characters which cannot be accurately represented in the target encoding when the //translit option is set for the target encoding" do
      # Hard to test this one, seems too implementation-dependent on
      # the platform's iconv(3) library
      Iconv.open "us-ascii//translit", "utf-16be" do |conv|
        conv.iconv("\xff\xfd").should == "?"
      end
    end
  end

  ruby_bug "#17910", "1.8.6.114" do
    it "sanitizes invalid upper bounds" do
      Iconv.open "us-ascii", "us-ascii" do |conv|
        conv.iconv("testing", 0, 99).should == "testing"
        conv.iconv("testing", 10, 12).should == ""
      end
    end
  end

  it "returns a blank string on invalid lower bounds" do
    Iconv.open "us-ascii", "us-ascii" do |conv|
      conv.iconv("testing", -10, -8).should == ""
      conv.iconv("testing", -8).should == ""
      conv.iconv("testing", -9, 5).should == ""
    end
  end
end

describe "Iconv.iconv" do
  it "converts a series of strings with a single converter" do
    # BOM only on first string
    Iconv.iconv("utf-16", "utf-8", "abc", "de").should equal_utf16(["\xfe\xff\0a\0b\0c", "\0d\0e"])
  end

  it "returns an empty array when given no strings to convert" do
    Iconv.iconv("us-ascii", "utf-8").should == []
  end

  it "acts exactly as if invoking Iconv#iconv consecutively on the same converter" do
    Iconv.iconv("utf-16", "utf-8", "a", "b", "c", nil, "d", "e").should equal_utf16(["\xfe\xff\0a", "\0b", "\0c", "", "\xfe\xff\0d", "\0e"])
  end

  it_behaves_like :iconv_initialize_exceptions, :iconv, "test"
end
