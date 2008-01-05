require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.executable?" do 
  before :each do
    @file1 = 'temp1.txt'
    @file2 = 'temp2.txt'

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){} # touch
    
    File.chmod(0755, @file1)
  end

  after :each do
    File.delete(@file1) if File.exist?(@file1)    
    File.delete(@file2) if File.exist?(@file2)

    @file1 =  nil
    @file2 = nil
  end

  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.executable?('fake_file').should == false
    File.executable?('/etc/passwd').should == false
    File.executable?(@file1).should == true
    File.executable?(@file2).should == false
  end

  platform_is_not :mswin do
    it "return true if the argument its an executable file" do
      File.executable?(@file1).should == true
      File.executable?(@file2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raises an ArgumentError if not passed one argument" do
    lambda { File.executable?        }.should raise_error(ArgumentError)
  end
  
  it "raises a TypeError if not passed a String type" do
    lambda { File.executable?(1)     }.should raise_error(TypeError)
    lambda { File.executable?(nil)   }.should raise_error(TypeError)
    lambda { File.executable?(false) }.should raise_error(TypeError)
  end
end
