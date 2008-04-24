require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#popen" do
  it "reads from a read-only pipe" do
    IO.popen("echo foo", "r") do |pipe|
      pipe.read.should == "foo\n"
    end
  end

  platform_is_not :windows do
    it "writes to a read/write pipe" do
      IO.popen("cat", "r+") do |pipe|
        pipe.write("bar")
        pipe.read 3
      end.should == "bar"
    end
  end

  it "with block does not raise error when io closed inside the block" do
    lambda {
      @io = IO.popen(RUBY_NAME, "r+") { |io| io.close; io }
    }.should_not raise_error
    @io.closed?.should == true
  end
end

describe "IO.popen" do
  it "needs to be reviewed for spec completeness" do
  end
end
