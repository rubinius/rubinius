require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.zero?" do
  before :each do
    @zero_file    = 'test.txt'
    @nonzero_file = 'test2.txt'
      
    File.open(@zero_file, "w") {} # Touch
    File.open(@nonzero_file, "w") { |f| f.puts "hello" }
  end
  
  after :each do
    File.delete(@zero_file) if File.exists?(@zero_file)
    File.delete(@nonzero_file) if File.exists?(@nonzero_file)  
    @zero_file    = nil
    @nonzero_file = nil
  end
     
  it "return true if the length of a file its zero, otherwise true" do
    File.zero?(@zero_file).should == true
    File.zero?(@nonzero_file).should == false
  end
   
  platform :mswin do
    it "returns true for NUL" do
      File.zero?('NUL').should == true
    end
  end
    
  platform :not, :mswin do
    it "returns true for /dev/null" do
      File.zero?('/dev/null').should == true
    end
  end
  
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do  
    lambda { File.zero?        }.should raise_error(ArgumentError)
    lambda { File.zero?(nil)   }.should raise_error(TypeError)
    lambda { File.zero?(true)  }.should raise_error(TypeError)
    lambda { File.zero?(false) }.should raise_error(TypeError)
  end
   
  specify "zero? should return true if the named file exists and has a zero size." do
    begin
      File.zero?('fake_file').should == false
      file = '/tmp/i_exist'
      File.open(file,'w'){
        File.zero?(file).should == true
      }
    ensure
      File.delete(file) rescue nil
    end
  end
end 
