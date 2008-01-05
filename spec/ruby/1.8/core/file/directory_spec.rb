require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.directory?" do 
  before :each do
    platform_is :mswin do
      @dir  = "C:\\"
      @file = "C:\\winnt\\notepad.exe"
    end
    
    platform_is_not :mswin do
      @dir  = "/"
      @file = "/bin/ls"
    end
  end

  after :each do
    @dir = nil
  end
 
  it "return true if dir is a directory, otherwise return false" do
    File.directory?(@dir).should == true
    File.directory?(@file).should == false
  end
   
  it "raises an ArgumentError if not passed one argument" do
    lambda { File.directory?              }.should raise_error(ArgumentError)
    lambda { File.directory?(@dir, @file) }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if not passed a String type" do
    lambda { File.directory?(nil) }.should raise_error(TypeError)
  end
end
