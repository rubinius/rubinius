require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#printf" do
  before(:each) do
    @io = StringIO.new('')
  end

  it "performs format conversion" do
    @io.printf("%d %04x", 123, 123).should == nil
    @io.string.should == "123 007b"
  end
end
