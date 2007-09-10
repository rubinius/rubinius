require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.executable?" do 
  before(:each) do
    @file1 = 'temp1.txt'
    @file2 = 'temp2.txt'

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){} # touch
    
    File.chmod(0755, @file1)
  end

  after(:each) do
    File.delete("temp1.txt")    
    File.delete("temp2.txt")

    @file1 =  nil
    @file2 = nil
  end

  unless WINDOWS
    it "return true if the argument its an executable file" do
      File.executable?(@file1).should == true
      File.executable?(@file2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raise an exception its the arguments are the worng type or number" do
    should_raise(ArgumentError){ File.executable? }
    should_raise(TypeError){ File.executable?(1) }
    should_raise(TypeError){ File.executable?(nil) }
    should_raise(TypeError){ File.executable?(false) }
  end
end

describe "File.executable?" do 
  before(:each) do
    @file1 = File.join('test.txt')
    @file2 = File.join('test2.txt')

    File.open(@file1, "w"){} # touch
    File.open(@file2, "w"){}
    
    File.chmod(0755, @file1)
  end

  after(:each) do
    File.delete(@file1)    
    File.delete(@file2) 
    @file1 =  nil
    @file2 = nil
  end
  
  it "returns true if named file is readable by the effective user id of the process, otherwise false" do
    File.executable?('fake_file').should == false
    File.executable?('/etc/passwd').should == false
    File.executable?(@file).should == true
  end

  unless WINDOWS
    it "return true if the argument its an executable file" do
      File.executable?(@file1).should == true
      File.executable?(@file2).should == false
      File.executable?('a_fake_file').should == false
    end
  end

  it "raise an exception its the arguments are the worng type or number" do
    should_raise(ArgumentError){ File.executable? }
    should_raise(TypeError){ File.executable?(1) }
    should_raise(TypeError){ File.executable?(nil) }
    should_raise(TypeError){ File.executable?(false) }
  end
end
