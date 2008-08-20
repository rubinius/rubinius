require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/equal_utf16'

describe EqualUtf16Matcher do
  it "when given strings, matches when actual == expected" do
    EqualUtf16Matcher.new("abcd").matches?("abcd").should == true
  end

  it "when given strings, matches when actual == expected, with byte order reversed" do
    EqualUtf16Matcher.new("abcd").matches?("badc").should == true
  end

  it "when given arrays, matches when actual == expected" do
    EqualUtf16Matcher.new(["abcd"]).matches?(["abcd"]).should == true
    EqualUtf16Matcher.new(["abcd", "efgh"]).matches?(["abcd", "efgh"]).should == true
  end

  it "when given arrays, matches when actual == a version of expected with byte order reversed for all strings contained" do
    EqualUtf16Matcher.new(["abcd"]).matches?(["badc"]).should == true
    EqualUtf16Matcher.new(["abcd", "efgh"]).matches?(["badc", "fehg"]).should == true
  end

  it "when given strings, does not match when actual != expected AND != expected with byte order reversed" do
    EqualUtf16Matcher.new("abcd").matches?("").should == false
    EqualUtf16Matcher.new("abcd").matches?(nil).should == false
    EqualUtf16Matcher.new("abcd").matches?("acbd").should == false
  end

  it "when given arrays, does not match when actual is not == expected or == a version of expected with byte order reversed for all strings contained simultaneously" do
    EqualUtf16Matcher.new(["abcd"]).matches?([]).should == false
    EqualUtf16Matcher.new(["abcd"]).matches?(["dcba"]).should == false
    EqualUtf16Matcher.new(["abcd", "efgh"]).matches?(["abcd", "fehg"]).should == false
  end

  it "provides a useful failure message" do
    matcher = EqualUtf16Matcher.new("a\0b\0")
    matcher.matches?("a\0b\0c\0")
    matcher.failure_message.should == ["Expected \"a\\000b\\000c\\000\"\n", "to equal \"a\\000b\\000\"\n or \"\\000a\\000b\"\n"]
  end

  it "provides a useful negative failure message" do
    matcher = EqualUtf16Matcher.new("a\0b\0")
    matcher.matches?("\0a\0b")
    matcher.negative_failure_message.should == ["Expected \"\\000a\\000b\"\n", "not to equal \"a\\000b\\000\"\n nor \"\\000a\\000b\"\n"]
  end
end
