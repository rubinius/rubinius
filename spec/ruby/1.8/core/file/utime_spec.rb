require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.utime" do
  before :each do
    @atime = Time.now
    @mtime = Time.now
    @file1 = "/tmp/specs_file_utime1"
    @file2 = "/tmp/specs_file_utime2"
    File.open(@file1, "w") {}
    File.open(@file2, "w") {}
  end
  
  after :each do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2)
  end
  
  it "sets the access and modification time of each file" do
    File.utime(@atime, @mtime, @file1, @file2)
    File.atime(@file1).to_i.should == @atime.to_i
    File.mtime(@file1).to_i.should == @mtime.to_i
    File.atime(@file2).to_i.should == @atime.to_i
    File.mtime(@file2).to_i.should == @mtime.to_i
  end
end
