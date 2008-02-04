require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#inspect" do
  it "does not raise anything when invoked on closed stream" do
    lambda { IOSpecs.closed_file.inspect }.should_not raise_error
  end

  it "returns string that contains 'closed' when invoked on closed stream" do
    IOSpecs.closed_file.inspect.should =~ /closed/
  end
end
