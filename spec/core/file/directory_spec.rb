require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.directory?" do 
  before(:each) do
    platform :mswin do
      @dir  = "C:\\"
      @file = "C:\\winnt\\notepad.exe"
    end
    
    platform :not, :mswin do
      @dir  = "/"
      @file = "/bin/ls"
    end
  end

  after(:each) do
    @dir = nil
  end
 
  it "return true if dir is a directory, otherwise return false" do
    File.directory?(@dir).should == true
    File.directory?(@file).should == false
  end
   
  it "raise an exception its the arguments are the worng type or number" do
    should_raise(ArgumentError){ File.directory? }
    should_raise(ArgumentError){ File.directory?(@dir, @file) }
    should_raise(TypeError){ File.directory?(nil) }
  end
end
