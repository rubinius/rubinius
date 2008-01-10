require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#each_byte" do
  before(:each) do
    @io = StringIO.new("xyz")
  end

  it "yields each character code in turn" do
    seen = []
    @io.each_byte {|b| seen << b}.should == nil
    seen.should == [120, 121, 122]
  end

  it "raises an IOError unless the IO is open for reading" do
    @io.close_read
    lambda { @io.each_byte {|b| b } }.should raise_error(IOError)
  end
end
