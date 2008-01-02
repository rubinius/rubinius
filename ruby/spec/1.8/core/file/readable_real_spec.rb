require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.readable_real?" do
  before :each do
    @file = '/tmp/i_exist'
  end
  
  after :each do
    File.delete(@file) if File.exists?(@file)
  end
  
  it "returns true if named file is readable by the real user id of the process, otherwise false" do
    File.readable_real?('fake_file').should == false
    File.open(@file,'w'){
      File.readable_real?(@file).should == true
    }
  end
end
