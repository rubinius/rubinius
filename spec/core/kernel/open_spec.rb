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
    @io.class.should == IO
  end
  
  it "should open an io when called with a block" do
    @output = open("|date") { |f| f.gets }
    @output.should_not == ''
  end
    
end