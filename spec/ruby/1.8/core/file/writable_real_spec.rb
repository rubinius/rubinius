require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.writable_real?" do  
  before :each do
    @file = '/tmp/i_exist'
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
    
  it "returns true if named file is writable by the real user id of the process, otherwise false" do
    File.writable_real?('fake_file').should == false
    File.open(@file,'w'){
      File.writable_real?(@file).should == true
    }
  end
  
  it "raises an ArgumentError if not passed one argument" do
    lambda { File.writable_real?        }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if not passed a String type" do
    lambda { File.writable_real?(1)     }.should raise_error(TypeError)
    lambda { File.writable_real?(nil)   }.should raise_error(TypeError)
    lambda { File.writable_real?(false) }.should raise_error(TypeError)
  end 
end
