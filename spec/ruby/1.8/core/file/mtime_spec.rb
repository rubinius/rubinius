require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.mtime" do
  before :each do
    @filename = '/tmp/i_exist'
    File.open(@filename, 'w') { @mtime = Time.now }
  end

  after :each do
    File.delete(@filename) if File.exist?(@filename)
  end

  it "returns the modification Time of the file" do
    File.mtime(@filename).class.should == Time
    File.mtime(@filename).should be_close(@mtime, 1.0)
  end

  it "raises an Errno::ENOENT exception if the file is not found" do
    lambda { File.mtime('bogus') }.should raise_error(Errno::ENOENT)
  end
end
