require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#inspect" do
  it "returns a String describing a stream" do
    STDOUT.inspect.should be_kind_of(String)
  end
end
