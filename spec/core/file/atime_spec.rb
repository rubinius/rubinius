require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.atime" do
  before(:each) do
    @file = File.join('test.txt')
    File.open(@file, "w"){} # touch
  end

  it "returns the last access time for the named file as a Time object" do      
    File.atime(@file)
    File.atime(@file).class.should == Time
  end

  it "raise an Errno::ENOENT exception if the file is not found" do 
    should_raise(Errno::ENOENT){ File.atime('a_fake_file') }
  end
   
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.atime }
    should_raise(ArgumentError){ File.atime(@file, @file) }
    should_raise(TypeError){ File.atime(1) }
  end
  
  after(:each) do 
    File.delete("test.txt") if File.exist?("test.txt") 
    @file = nil
  end
end

describe "File#atime" do
  before(:each) do
    @name = File.expand_path(__FILE__)
    @file = File.open(@name)
  end

  after(:each) do 
    @name = nil
    @file = nil
  end

  it "returns the last access time to self" do
    @file.atime  
    @file.atime.class.should == Time
  end

  it "raise an Exception if it has the worng number of argments" do
    should_raise(ArgumentError){ @file.atime(@name) }
  end
end
