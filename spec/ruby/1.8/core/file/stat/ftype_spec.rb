require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat.ftype" do
  it "returns file for a file" do
    file = "/tmp/i_am_file"
    File.open(file, "w") {}
    File.stat(file).ftype.should == "file"
  end

  it "returns directory for a dir" do
    File.stat('/tmp').ftype.should == "directory"
  end
  #add in tests for characterSpecial, blockSpecial, fifo, link, and socket
end


