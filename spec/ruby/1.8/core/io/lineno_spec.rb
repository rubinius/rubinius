require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#lineno" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.lineno }.should raise_error(IOError)
  end
end
