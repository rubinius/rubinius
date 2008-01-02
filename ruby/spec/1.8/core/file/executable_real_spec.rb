require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.executable_real?" do
  before :each do
    @file1 = File.join(Dir.pwd, 'temp1.txt')
    @file2 = File.join(Dir.pwd, 'temp2.txt')

    File.open(@file1, "w") {} # touch
    File.open(@file2, "w") {}
      
    File.chmod(0755, @file1)
  end
 
  after :each do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2)

    @file1 = nil
    @file2 = nil
  end 
 
  platform_is_not :mswin do
    it "returns true if the file its an executable" do 
      File.executable_real?(@file1).should == true
      File.executable_real?(@file2).should == false
      File.executable_real?('bogus').should == false
    end
  end
  
  it "raise an exception if the argument is not from the correct type or are missing" do
    lambda { File.executable_real?        }.should raise_error(ArgumentError)
    lambda { File.executable_real?(1)     }.should raise_error(TypeError)
    lambda { File.executable_real?(nil)   }.should raise_error(TypeError)
    lambda { File.executable_real?(false) }.should raise_error(TypeError)
  end
end

describe "File.executable_real?" do
  before :each do
    @file1 = 'test.txt'
    @file2 = 'test2.txt'

    File.open(@file1, "w") {} # touch
    File.open(@file2, "w") {}
      
    File.chmod(0755, @file1)
  end
  
  after :each do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2) 
    @file1 = nil
    @file2 = nil
  end 
 
  it "returns true if named file is readable by the real user id of the process, otherwise false" do
    File.executable_real?('fake_file').should == false
    File.executable_real?(@file1).should == true
  end
  
  platform_is_not :mswin do
    it "returns true if the file its an executable" do 
      File.executable_real?(@file1).should == true
      File.executable_real?(@file2).should == false
      File.executable_real?('bogus').should == false
    end
  end
  
  it "raise an exception if the argumnent is not from the correct type or are missing" do
    lambda { File.executable_real?        }.should raise_error(ArgumentError)
    lambda { File.executable_real?(1)     }.should raise_error(TypeError)
    lambda { File.executable_real?(nil)   }.should raise_error(TypeError)
    lambda { File.executable_real?(false) }.should raise_error(TypeError)
  end
end   
