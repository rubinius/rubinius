require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/tell'

describe "StringIO#pos" do
  it_behaves_like :stringio_tell, :pos
end

describe "StringIO#pos=" do
  before(:each) do
    @io = StringIOSpecs.build
  end

  it "updates the current byte offset" do
    @io.pos = 26
    @io.read(1).should == "r"
  end

  it "raises an EINVAL if given a negative argument" do
    lambda { @io.pos = -10 }.should  raise_error(Errno::EINVAL)
  end
end
