require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#binmode" do
  it "does not raise any errors on closed stream" do
    lambda { IOSpecs.closed_file.binmode }.should_not raise_error()
  end
end
