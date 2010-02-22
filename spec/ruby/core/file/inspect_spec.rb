require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../io/fixtures/classes', __FILE__)

describe "File#inspect" do
  it "returns string that contains 'closed' when invoked on closed file" do
    IOSpecs.closed_file.inspect.should =~ /closed/
  end
end
