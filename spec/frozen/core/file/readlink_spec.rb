require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.readlink" do

  before :each do
    @file1 = 'test.txt'
    @file3 = 'test.lnk'
    File.delete(@file3) if File.exists?(@file3)

    File.open(@file1, 'w+') { } #
    File.symlink(@file1, @file3)
  end

  after :each do
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file3) if File.symlink?(@file3)
  end

  it "return the name of the file referenced by the given link" do
    File.readlink(@file3).should == @file1
  end

  it "raises an Errno::ENOENT if called with an invalid argument" do
    lambda { File.readlink("/this/surely/doesnt/exist") }.should raise_error(Errno::ENOENT)
  end
end
