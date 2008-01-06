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
  
  it "reads the contents of a file" do
    IO.read(@fname).should == @contents
  end
  
  it "reads the contents of a file up to a certain size when specified" do
    IO.read(@fname, 5).should == @contents.slice(0..4)
  end
    
  it "reads the contents of a file from an offset of a specific size when specified" do
    IO.read(@fname, 5, 3).should == @contents.slice(3, 5)
  end
  
  it "raises an Errno::ENOENT when the requested file does not exist" do
    File.delete(@fname) if File.exists?(@fname)
    lambda { IO.read @fname }.should raise_error(Errno::ENOENT)
  end
  
  it "raises a TypeError when not passed a String type" do
    lambda { IO.read nil }.should raise_error(TypeError)
  end
  
  it "raises an ArgumentError when not passed a valid length" do
    lambda { IO.read @fname, -1 }.should raise_error(ArgumentError)
  end
  
  it "raises an Errno::EINVAL when not passed a valid offset" do
    lambda { IO.read @fname, 0, -1  }.should raise_error(Errno::EINVAL)
    lambda { IO.read @fname, -1, -1 }.should raise_error(Errno::EINVAL)
  end
end

describe "IO#read" do

  before :each do
    @fname = "test.txt"
    @contents = "1234567890"
    open @fname, "w" do |io| io.write @contents end

    @io = open @fname, "r+"
  end

  after :each do
    @io.close
    File.delete(@fname) if File.exists?(@fname)
  end

  it "is at end-of-file when everything has been read" do
    @io.read
    @io.eof?.should == true
  end

  it "reads the contents of a file" do
    @io.read.should == @contents
  end

  it "returns an empty string at end-of-file" do
    @io.read
    @io.read.should == ''
  end

  it "reads the contents of a file when more bytes are specified" do
    @io.read(@contents.length + 1).should == @contents
  end

  it "returns an empty string at end-of-file" do
    @io.read
    @io.read.should == ''
  end

  it "returns nil at end-of-file with a length" do
    @io.read
    @io.read(1).should == nil
  end

end

