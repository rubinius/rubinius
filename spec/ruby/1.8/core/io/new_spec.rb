require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.new" do
  before :each do
    @file = File.open("/tmp/test-file", "w+")
  end
  after :each do
    @file.close rescue nil
  end

  it "raises a TypeError if given invalid arguments" do
    lambda { IO.new(nil) }.should raise_error(TypeError)
  end

  it "warns and ignores the provided block" do
    begin
      lambda {
        @var = "block_ignored"
        @io = IO.new(@file.fileno) { |io| @var = "block_executed" ; io}
      }.should complain(/IO::new.*does not take block.*IO::open.*instead/)
      @var.should == "block_ignored"
      @io.closed?.should == false
    ensure
      @io.close unless @io.closed?
      @io = nil
    end
  end
end
