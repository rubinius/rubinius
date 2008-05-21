require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#flush" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.flush }.should raise_error(IOError)
  end
end
