require File.dirname(__FILE__) + '/../../../spec_helper'

describe "CType#isspace" do
  it "returns true if self is ASCII whitespace" do
    a = []
    "\tt\fi a\r\nt\v".each_byte { |b| a << b.isspace }
    a.should == [true, false, true, false, true, false, true, true, false, true]
  end
end
