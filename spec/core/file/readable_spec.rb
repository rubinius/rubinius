require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.readable?" do
  before(:each) do
    @file = '/tmp/i_exist'
  end
  
  after(:each) do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.readable?('fake_file').should == false
    File.readable?('/etc/passwd').should == true
    File.open(@file,'w'){
      File.readable?(@file).should == true
    }
  end
end
