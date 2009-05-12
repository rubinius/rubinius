require File.dirname(__FILE__) + '/../spec_helper'

describe "The -e command line option" do
  it "evaluates the given string" do
    ruby_exe("puts 'foo'").chomp.should == "foo"
  end

  it "joins multiple strings with newlines" do
    ruby_exe(nil, :args => %Q{-e "puts 'hello" -e "world'"}).chomp.should == "hello\nworld"
  end

  it "uses 'main' as self" do
    ruby_exe("puts self").chomp.should == "main"
  end

  it "uses '-e' as file" do
    ruby_exe("puts __FILE__").chomp.should == "-e"
  end

  #needs to test return => LocalJumpError
end
