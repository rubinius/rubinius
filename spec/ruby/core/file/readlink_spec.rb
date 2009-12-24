require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.readlink" do

  before :each do
    @file = tmp('file_readlink.txt')
    @link = tmp('file_readlink.lnk')

    touch @file

    rm_r @link
    File.symlink(@file, @link)
  end

  after :each do
    rm_r @link, @file
  end

  it "return the name of the file referenced by the given link" do
    File.readlink(@link).should == @file
  end

  it "raises an Errno::ENOENT if called with an invalid argument" do
    # TODO: missing_file
    lambda { File.readlink("/this/surely/doesnt/exist") }.should raise_error(Errno::ENOENT)
  end
end
