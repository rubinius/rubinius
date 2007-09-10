require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#path" do
  before(:each) do
    @file1 = "testfile"
    @file2 = "/tmp/../tmp/xxx"
  end
  
  after(:each) do
    File.delete(@file1) rescue nil
    File.delete(@file2) rescue nil
  end
  
  it "should return the pathname used to create file as a string" do
    File.open(@file1,'w'){|file| file.path.should == "testfile"}
    File.open(@file2, 'w'){|file| file.path.should == "/tmp/../tmp/xxx"}
  end
end
