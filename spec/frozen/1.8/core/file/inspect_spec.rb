require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../io/fixtures/classes'

describe "File#inspect" do
  it "returns string that contains 'closed' when invoked on closed file" do
    IOSpecs.closed_file.inspect.should =~ /closed/
  end
end
