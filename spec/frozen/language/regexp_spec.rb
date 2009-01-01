require File.dirname(__FILE__) + '/../spec_helper'

describe "Literal Regexps" do
  it "matches against $_ (last input) in a conditional if no explicit matchee provided" do
    $_ = nil

    (true if /foo/).should_not == true

    $_ = "foo"

    (true if /foo/).should == true
  end

  it "allow substitution of strings" do
    str = "o?$"
    (/fo#{str}/ =~ "foo?").should == nil
    (/fo#{str}/ =~ "foo").should == 0
  end

  it "allow substitution of literal regexps" do
    str = /o?$/
    (/fo#{str}/ =~ "foo?").should == nil
    (/fo#{str}/ =~ "foo").should == 0
  end
  
  it "matches . to newlines if Regexp::MULTILINE is specified" do
    str = "abc\ndef"
    (str =~ /.def/).should == nil 
    (str =~ /.def/m).should == 3
  end
end
