require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#stat" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.stat }.should raise_error(IOError)
  end

  it "returns a File::Stat object" do
    io = IO.new STDERR.fileno

    io.stat.class.should == File::Stat
  end
end
