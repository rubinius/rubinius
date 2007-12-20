require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#open" do
  before :each do
    @file = "test.txt"
    File.open(@file, "w"){ |f| f.puts "This is a test" }
  end
  
  after :each do
    File.delete(@file) rescue nil
  end
  
  it "should open a file when given a valid filename" do
    @file = open("test.txt")
    @file.class.should == File
  end
  
  it "should open a file when called with a block" do
    @output = open("test.txt", "r") { |f| f.gets }
    @output.should == "This is a test\n"
  end
  
  it "should open an io when path starts with a pipe" do
    @io = open("|date")
    @io.should be_kind_of(IO)
  end
  
  it "should open an io when called with a block" do
    @output = open("|date") { |f| f.gets }
    @output.should_not == ''
  end
  
  it "raise an exception if the arguments are not of the correct type or are missing" do
    lambda { open            }.should raise_error(ArgumentError)
    lambda { open(nil)       }.should raise_error(TypeError)
    lambda { open(7)         }.should raise_error(TypeError)
    lambda { open(mock('x')) }.should raise_error(TypeError)
  end
end