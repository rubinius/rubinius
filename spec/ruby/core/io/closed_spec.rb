require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#closed?" do
  before :each do
    @io = IOSpecs.io_fixture "lines.txt"
  end

  after :each do
    @io.close
  end

  it "returns true on closed stream" do
    IOSpecs.closed_file.closed?.should be_true
  end

  it "returns false on open stream" do
    @io.closed?.should be_false
  end
end
