require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#stat" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.stat }.should raise_error(IOError)
  end
end
