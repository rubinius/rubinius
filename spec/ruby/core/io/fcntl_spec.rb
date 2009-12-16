require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#fcntl" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.fcntl(5, 5) }.should raise_error(IOError)
  end
end
