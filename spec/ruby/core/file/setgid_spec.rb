require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/setgid'
 
describe "File.setgid?" do
  it_behaves_like :file_setgid, :setgid?, File
end
 
describe "File.setgid?" do
  before(:each) do
    @name = tmp('test.txt')
    touch @name
  end
  
  after(:each) do
    rm_r @name
  end
  
  it "should return false if the file was just made" do
    File.setgid?(@name).should == false
  end
  
  it "should be false if the file doesn't exist" do
    rm_r @name # delete it prematurely, just for this part
    File.setgid?(@name).should == false
  end
  
  platform_is_not :windows do
    it "should return true when the gid bit is set" do
      system "chmod g+s #{@name}"
      
      File.setgid?(@name).should == true
    end
  end
end
