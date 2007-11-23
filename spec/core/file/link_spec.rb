require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.link" do
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

  platform :not, :mswin do
    it "link a file with another" do
      File.link(@file, @link).should == 0
      File.exists?(@link).should == true
      File.identical?(@file, @link).should == true
    end

    it "raise an exception if the target already exists" do
      File.link(@file, @link)  
      should_raise(Errno::EEXIST){ File.link(@file, @link) }
    end

    it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
      should_raise(ArgumentError){ File.link }
      should_raise(ArgumentError){ File.link(@file) }
    end
  end
end
