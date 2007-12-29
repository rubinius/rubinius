require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.symlink" do
  before :each do   
    @file = "test.txt"
    @link = "test.lnk"     
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    File.open(@file,"w+") 
  end 

  after :each do
    File.unlink(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    @link = nil
  end

  platform_is_not :mswin do
    it "create a symlink between a source and target file" do
      File.symlink(@file, @link).should == 0
      File.exists?(@link).should == true
      File.identical?(@file, @link).should == true
    end
    
    it "create a symbolic link" do
      File.symlink(@file, @link)
      File.symlink?(@link).should == true
    end
    
    it "raise an exception if the target already exists" do
      File.symlink(@file, @link)  
      lambda { File.symlink(@file, @link) }.should raise_error(Errno::EEXIST)
    end
    
    it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
      lambda { File.symlink        }.should raise_error(ArgumentError)
      lambda { File.symlink(@file) }.should raise_error(ArgumentError)
    end
  end
end
