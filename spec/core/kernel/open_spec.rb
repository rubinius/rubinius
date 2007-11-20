require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#open" do
  before :each do
    @file = "test.txt"
    File.open(@file, "w"){} # touch
  end
  
  after :each do
    File.delete(@file) rescue nil
  end
  
  it "should open a file when given a valid filename" do
    @file = open("test.txt")
    @file.class.should == File
  end
  
  it "should open an io when path starts with a pipe" do
    @io = open("|date")
    @io.class.should == IO
  end
    
end