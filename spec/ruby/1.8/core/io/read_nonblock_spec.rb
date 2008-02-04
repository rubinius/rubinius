require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#read_nonblock" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.read_nonblock(5) }.should raise_error(IOError)
  end
end
