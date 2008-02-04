require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#putc" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.putc('a') }.should raise_error(IOError)
  end
end
