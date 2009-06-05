require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

ruby_version_is '1.8.7' do
  describe "String#rpartition with String" do
    it "returns an array of substrings based on splitting on the given string" do
      "hello world".rpartition("o").should == ["hello w", "o", "rld"]
    end

    it "always returns 3 elements" do
      "hello".rpartition("x").should == ["", "", "hello"]
      "hello".rpartition("hello").should == ["", "hello", ""]
    end

    it "accepts regexp" do
      "hello!".rpartition(/l./).should == ["hel", "lo", "!"]
    end

    ruby_bug "redmine #1510", '1.9.1' do
      it "converts its argument using :to_str" do
        find = mock('l')
        find.should_receive(:to_str).and_return("l")
        "hello".rpartition(find).should == ["hel","l","o"]
      end
    end

    it "raises error if not convertible to string" do
      lambda{ "hello".rpartition(5) }.should raise_error(TypeError)
      lambda{ "hello".rpartition(nil) }.should raise_error(TypeError)
    end
  end
end