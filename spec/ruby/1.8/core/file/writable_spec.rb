require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.writable?" do
  before :each do
    @file = '/tmp/i_exist'
  end
  
  after :each do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "returns true if named file is writable by the effective user id of the process, otherwise false" do
    File.writable?('fake_file').should == false
    File.writable?('/etc/passwd').should == false
    File.open(@file,'w'){
      File.writable?(@file).should == true
    }
  end
end
