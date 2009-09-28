require File.dirname(__FILE__) + '/../../spec_helper'
require File.expand_path(File.dirname(__FILE__) + '/../fixtures/classes')

describe "Regexps with interpolation" do

  it "allow interpolation of strings" do
    str = "foo|bar"
    /#{str}/.should == /foo|bar/
  end

  it "allows interpolation of literal regexps" do
    re = /foo|bar/
    /#{re}/.should == /(?-mix:foo|bar)/
  end
  
  it "allows interpolation of any class that responds to to_s" do
    o = LanguageSpecs::ClassWith_to_s.new
    /#{o}/.should == /class_with_to_s/
  end
  
  it "throws NoMethodError on missing to_s" do
    o = LanguageSpecs::ClassWithout_to_s.new
    lambda { /#{o}/ }.should raise_error(NoMethodError)
  end
  
  it "allows interpolation which mixes modifiers" do
    re = /foo/i
    /#{re} bar/m.should == /(?i-mx:foo) bar/m
  end
  
  it "allows interpolation to interact with other Regexp constructs" do
    str = "foo)|(bar"
    /(#{str})/.should == /(foo)|(bar)/
    
    str = "a"
    /[#{str}-z]/.should == /[a-z]/
  end
  
  it "gives precedence to escape sequences over substitution" do
    str = "J"
    /\c#{str}/.to_s.should == '(?-mix:\c#' + '{str})'
  end  

  it "throws RegexpError for malformed interpolation" do
    s = ""
    lambda { /(#{s}/ }.should raise_error(RegexpError)
    s = "("
    lambda { /#{s}/ }.should raise_error(RegexpError)
  end

  it "allows interpolation in extended mode" do
    var = "#comment\n  foo  #comment\n  |  bar"
    (/#{var}/x =~ "foo").should == (/foo|bar/ =~ "foo")
  end
end
