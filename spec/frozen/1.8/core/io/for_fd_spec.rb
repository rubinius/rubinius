require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#for_fd" do
  it "raises IOError on closed stream" do
    lambda { IO.for_fd(IOSpecs.closed_file.fileno) }.should raise_error(IOError)
  end
end

describe "IO.for_fd" do
  it "needs to be reviewed for spec completeness"
end
