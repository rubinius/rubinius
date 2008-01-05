require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.file?" do 
  before :each do 
    platform_is :mswin do
      @null = "NUL"
      @dir  = "C:\\"
    end
    
    platform_is_not :mswin do
      @null = "/dev/null"
      @dir  = "/bin"
    end

    @file = "test.txt"
    File.open(@file, "w"){} # touch
  end

  after :each do
    File.delete(@file) rescue nil
    @null = nil
    @file = nil
  end
  
  it "returns true if the named file exists and is a regular file." do 
    File.file?(@file).should == true
    File.file?(@dir).should == false
    File.file?(@null).should == false # May fail on MS Windows
  end

  it "raises an ArgumentError if not passed one argument" do
    lambda { File.file?               }.should raise_error(ArgumentError)
    lambda { File.file?(@null, @file) }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if not passed a String type" do
    lambda { File.file?(nil) }.should raise_error(TypeError)
    lambda { File.file?(1)   }.should raise_error(TypeError)
  end
end
