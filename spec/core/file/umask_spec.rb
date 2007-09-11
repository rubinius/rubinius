require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.umask" do
  before :each do
    @file = 'test.txt'
    File.open(@file, 'w'){}
  end
   
  it "return a Fixnum" do
    File.umask.class.should == Fixnum
  end    
  
  specify "umask should return the current umask value for the process" do
    File.umask(0006).should == 18
    File.umask.should == 6
  end
   
  platform :mswin do
    it "Returns the current umask value for this process. (basic)" do   
      File.umask.should == 0
    end
      
    # The value used here is the value of _S_IWRITE.
    it "Returns the current umask value for this process." do 
      File.umask(0000200) 
      File.umask.should == 128
    end
      
    it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do  
      File.umask(0006)  
      File.umask.should == 0
    end
  end

  after :each do
    File.delete(@file) if File.exists?(@file)
    @file = nil
  end
end 
