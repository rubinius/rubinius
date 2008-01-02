require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.read" do

  before :each do
    @fname = "test.txt"
    @contents = "1234567890"
    File.open(@fname, "w") { |f| f.write(@contents) }
  end
  
  after :each do
    File.delete(@fname) if File.exists?(@fname)
  end
  
  it "should read the contents of a file" do
    IO.read(@fname).should == @contents
  end
  
  it "should read the contents of a file up to a certain size when specified" do
    IO.read(@fname, 5).should == @contents.slice(0..4)
  end
    
  it "should read the contents of a file from an offset of a specific size when specified" do
    IO.read(@fname, 5, 3).should == @contents.slice(3, 5)
  end
  
  it "should throw an exception if the requested file does not exist" do
    File.delete(@fname) if File.exists?(@fname)
    lambda { IO.read @fname }.should raise_error(Errno::ENOENT)
  end
  
  it "should throw an exception if given the wrong type or number of arguments" do
    lambda { IO.read nil }.should raise_error(TypeError)
    lambda { IO.read @fname, -1 }.should raise_error(ArgumentError)
    lambda { IO.read @fname, 0, -1  }.should raise_error(Errno::EINVAL)
    lambda { IO.read @fname, -1, -1 }.should raise_error(Errno::EINVAL)
  end
end
