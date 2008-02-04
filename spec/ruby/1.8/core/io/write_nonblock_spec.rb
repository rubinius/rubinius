require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#write_nonblock" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.write_nonblock("hello") }.should raise_error(IOError)
  end
end
