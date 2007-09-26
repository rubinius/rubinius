require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.unlink" do
  before :each  do
    @file = "i_dont_exist"
    File.delete(@file) if File.exists?(@file)
  end
  
  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "should delete a file and return the number of names passed as arguments" do
    File.exists?(@file).should == false
    File.new(@file,'w').close
    File.exists?(@file).should == true
    File.unlink(@file).should == 1
    File.exists?(@file).should == false 
  end
  
  it "should raise Errno::ENOENT if the file does not exist" do
    should_raise(Errno::ENOENT){ File.unlink('bogus') }
  end
end 
