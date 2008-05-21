require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#binmode" do
  it "returns the IO object" do
    @io = StringIO.new("example")
    @io.binmode.should == @io
  end
end
