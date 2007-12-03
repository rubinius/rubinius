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

  platform :not, :mswin do
    it "return true if the argument its an executable file" do
      File.executable?(@file1).should == true
      File.executable?(@file2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raise an exception its the arguments are the worng type or number" do
    lambda { File.executable?     ``   }.should raise_error(ArgumentError)
    lambda { File.executable?(1)     }.should raise_error(TypeError)
    lambda { File.executable?(nil)   }.should raise_error(TypeError)
    lambda { File.executable?(false) }.should raise_error(TypeError)
  end
end

describe "File.executable?" do 
  before :all do
    @file_name_1 = "test_1.txt"
    @file_name_2 = "test_2.txt"
    @file1 = File.join(@file_name_1)
    @file2 = File.join(@file_name_2)

    File.open(@file1, "w") {} # touch
    File.open(@file2, "w") {}
    
    File.chmod(0755, @file1)
  end

  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.executable?('fake_file').should == false
    File.executable?('/etc/passwd').should == false
    File.executable?(@file_name_1).should == true
    File.executable?(@file_name_2).should == false
  end

  platform :not, :mswin do
    it "return true if the argument its an executable file" do
      File.executable?(@file_name_1).should == true
      File.executable?(@file_name_2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raise an exception its the arguments are the wrong type or number" do
    lambda { File.executable?        }.should raise_error(ArgumentError)
    lambda { File.executable?(1)     }.should raise_error(TypeError)
    lambda { File.executable?(nil)   }.should raise_error(TypeError)
    lambda { File.executable?(false) }.should raise_error(TypeError)
  end

  after :all do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2) 
    @file1 =  nil
    @file2 = nil
  end
end
