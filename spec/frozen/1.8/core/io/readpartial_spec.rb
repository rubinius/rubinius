require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readpartial" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.readpartial(10) }.should raise_error(IOError)
  end
end
