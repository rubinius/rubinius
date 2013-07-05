require File.expand_path('../../../spec_helper', __FILE__)

describe "ArgumentError" do
  it "uses its internal message in #inspect" do
    error_inspection = lambda { |e| e.inspect.should == "#<ArgumentError: method '+': given 2, expected 1>" }
    lambda { 1.+(2, 3) }.should raise_error(ArgumentError, &error_inspection)
  end
end
