require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.atime" do
  before :each do
    @file = File.join('test.txt')
    File.open(@file, "w") {} # touch
  end
  
  after :each do 
    File.delete(@file) if File.exist?(@file) 
    @file = nil
  end

  it "returns the last access time for the named file as a Time object" do      
    File.atime(@file)
    File.atime(@file).class.should == Time
  end

  it "raise an Errno::ENOENT exception if the file is not found" do 
    lambda { File.atime('a_fake_file') }.should raise_error(Errno::ENOENT)
  end
end

describe "File#atime" do
  before :each do
    @name = File.expand_path(__FILE__)
    @file = File.open(@name)
  end

  after :each do
    @file.close rescue nil
    @name = nil
    @file = nil
  end

  it "returns the last access time to self" do
    @file.atime  
    @file.atime.class.should == Time
  end
end
