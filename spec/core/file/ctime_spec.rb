require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.ctime" do
  before(:each) do
    @file = __FILE__
  end

  after(:each) do
    @file = nil
  end

  it "Returns the change time for the named file (the time at which directory information about the file was changed, not the file itself)." do 
    File.ctime(@file)  
    File.ctime(@file).class.should == Time
  end

  it "raise an Errno::ENOENT exception if the file is not found" do    
    should_raise(Errno::ENOENT){ File.ctime('bogus') }
  end
  
  it  "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.ctime }
    should_raise(ArgumentError){ File.ctime(@file, @file) }
    should_raise(TypeError){ File.ctime(1) }
  end
end 

describe "File#ctime" do
  before(:each) do
    @file = File.open(__FILE__)
  end

  after(:each) do
    @file = nil
  end

  it "Returns the change time for the named file (the time at which directory information about the file was changed, not the file itself)." do 
    @file.ctime
    @file.ctime.class.should == Time
  end 
  
  it  "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ @file.ctime(@file, @file) } 
  end
end 
