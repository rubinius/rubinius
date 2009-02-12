require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#open" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:open)
  end
  
  before :each do
    @file = "test.txt"
    File.open(@file, "w"){ |f| f.puts "This is a test" }
  end
  
  after :each do
    File.delete(@file) rescue nil
  end
  
  it "opens a file when given a valid filename" do
    @file = open("test.txt")
    @file.class.should == File
  end
  
  it "opens a file when called with a block" do
    @output = open("test.txt", "r") { |f| f.gets }
    @output.should == "This is a test\n"
  end
  
  platform_is_not :windows do
    
    it "opens an io when path starts with a pipe" do
      @io = open("|date")
      @io.should be_kind_of(IO)
    end
    
    it "opens an io when called with a block" do
      @output = open("|date") { |f| f.gets }
      @output.should_not == ''
    end
  
  end

  platform_is :windows do
    
    it "opens an io when path starts with a pipe" do
      @io = open("|date /t")
      @io.should be_kind_of(IO)
    end
    
    it "opens an io when called with a block" do
      @output = open("|date /t") { |f| f.gets }
      @output.should_not == ''
    end
  
  end
    
  
  it "raises an ArgumentError if not passed one argument" do
    lambda { open }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if not passed a String type" do
    lambda { open(nil)       }.should raise_error(TypeError)
    lambda { open(7)         }.should raise_error(TypeError)
    lambda { open(mock('x')) }.should raise_error(TypeError)
  end
end

describe "Kernel.open" do
  it "needs to be reviewed for spec completeness"
end
