require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO.pipe" do
  it "creates a two-ended pipe" do
    r, w = IO.pipe
    w.puts "test_create_pipe\\n"
    w.close
    r.read(16).should == "test_create_pipe"
    r.close
  end

  it "returns two IO objects" do
    r,w = IO.pipe
    r.should be_kind_of(IO)
    w.should be_kind_of(IO)
  end

  it "returns instances of a subclass when called on a subclass" do
    r, w = IOSpecs::SubIO.pipe
    r.should be_an_instance_of(IOSpecs::SubIO)
    w.should be_an_instance_of(IOSpecs::SubIO)
    w.close
    r.close
  end
end


