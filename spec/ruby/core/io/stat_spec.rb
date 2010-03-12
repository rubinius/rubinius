require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#stat" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.stat }.should raise_error(IOError)
  end

  it "returns a File::Stat object for the stream" do
    io = IO.new $stderr.fileno
    io.stat.should be_an_instance_of(File::Stat)
  end
end
