require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#popen" do
  it "with block does not raise error when io closed inside the block" do
    lambda {
      @io = IO.popen(RUBY_NAME, "r+") { |io| io.close; io }
    }.should_not raise_error
    @io.closed?.should == true
  end
end
