require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#open" do
  before :each do
    @file = File.open("/tmp/test-file", "w+")
  end
  after :each do
    @file.close rescue nil
  end

  it "with block does not raise error when io closed inside the block" do
    lambda {
      @io = IO.open(@file.fileno) { |io| io.close; io }
    }.should_not raise_error
    @io.closed?.should == true
  end
end
