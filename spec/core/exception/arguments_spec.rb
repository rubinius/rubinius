require File.expand_path('../../../spec_helper', __FILE__)

describe "ArgumentError" do
  it "uses its internal message in #inspect" do
    lambda { 1.+(2, 3) }.should raise_error(ArgumentError) do |e|
      e.inspect.should == "#<ArgumentError: method '+': given 2, expected 1>"
    end
  end
end
