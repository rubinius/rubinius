require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'

describe "GetoptLong#set_options" do
  before(:each) do
    @opts = GetoptLong.new
  end
  
  it "should allow setting command line options" do
    begin
      old_argv_value = ARGV
      ARGV = ["--size", "10k", "-v", "arg1", "arg2"]
    
      @opts.set_options(
        ["--size", GetoptLong::REQUIRED_ARGUMENT],
        ["--verbose", "-v", GetoptLong::NO_ARGUMENT]
      )
    
      @opts.get.should == ["--size", "10k"]
      @opts.get.should == ["--verbose", ""]
      @opts.get.should == nil
    ensure
      ARGV = old_argv_value
    end
  end

  it "discards previously defined command line options" do
    begin
      old_argv_value = ARGV
      ARGV = ["--size", "10k", "-v", "arg1", "arg2"]
    
      @opts.set_options(
        ["--size", GetoptLong::REQUIRED_ARGUMENT],
        ["--verbose", "-v", GetoptLong::NO_ARGUMENT]
      )
    
      @opts.set_options(
        ["-s", "--size", GetoptLong::REQUIRED_ARGUMENT],
        ["-v", GetoptLong::NO_ARGUMENT]
      )
    
      @opts.get.should == ["-s", "10k"]
      @opts.get.should == ["-v", ""]
      @opts.get.should == nil
    ensure
      ARGV = old_argv_value
    end
  end
  
  it "should raise an ArgumentError if too many argument flags where given" do
    lambda {
      @opts.set_options(["--size", GetoptLong::NO_ARGUMENT, GetoptLong::REQUIRED_ARGUMENT])
    }.should raise_error(ArgumentError)
  end
  
  it "should raise a RuntimeError if processing has already started" do
    @opts.get
    lambda {
      @opts.set_options()
    }.should raise_error(RuntimeError)
  end
  
  it "should raise an ArgumentError if no argument flag was given" do
    lambda {
      @opts.set_options(["--size"])
    }.should raise_error(ArgumentError)
  end
  
  it "should raise an ArgumentError if one of the given arguments is not an Array" do
    lambda {
      @opts.set_options(
        ["--size", GetoptLong::REQUIRED_ARGUMENT],
        "test")
    }.should raise_error(ArgumentError)
  end
  
  it "should raise an ArgumentError if the same option is given twice" do
    lambda {
      @opts.set_options(
        ["--size", GetoptLong::NO_ARGUMENT],
        ["--size", GetoptLong::OPTIONAL_ARGUMENT])
    }.should raise_error(ArgumentError)

    lambda {
      @opts.set_options(
        ["--size", GetoptLong::NO_ARGUMENT],
        ["-s", "--size", GetoptLong::OPTIONAL_ARGUMENT])
    }.should raise_error(ArgumentError)
  end
  
  it "should raise an ArgumentError if the given option is invalid" do
    lambda {
      @opts.set_options(["-size", GetoptLong::NO_ARGUMENT])
    }.should raise_error(ArgumentError)
  end
end