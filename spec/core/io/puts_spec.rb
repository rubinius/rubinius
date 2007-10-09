require File.dirname(__FILE__) + '/../../spec_helper'

DEFAULT_RECORD_SEPARATOR = $/

# TODO: need to find a better way to test this. Too fragile to set expectations
# to each write call. Only care that all the characters are sent not the number
# or write calls. Also, these tests do not make sure the ordering of the write calls
# are correct.
describe "IO#puts" do
  before(:each) do
    @io = IO.new(2)
  end
  
  it "no args" do
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR])
    @io.puts.should == nil
  end
  
  it "nil arg" do
    @io.should_receive(:write, :with => ["nil"])
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR])
    @io.puts(nil).should == nil
  end
  
  it "non string args" do
    object = Object.new
    object.should_receive(:to_s, :returning => "hola")
    
    @io.should_receive(:write, :with => ["hola"])
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR])
    @io.puts(object).should == nil
  end
  
  it "multiple args" do
    @io.should_receive(:write, :with => ["1"])
    @io.should_receive(:write, :with => ["two"])
    @io.should_receive(:write, :with => ["3"])
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR], :count => 3)
    @io.puts(1, "two", 3).should == nil
  end
  
  it "nested array arg" do
    @io.should_receive(:write, :with => ["1"])
    @io.should_receive(:write, :with => ["2"])
    @io.should_receive(:write, :with => ["3"])
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR], :count => 3)
    @io.puts([1, 2, [3]]).should == nil
  end
  
  it "recursive array arg" do
    x = []
    x << 2 << x
    @io.should_receive(:write, :with => ["2"])
    @io.should_receive(:write, :with => ["[...]"])
    @io.should_receive(:write, :with => [DEFAULT_RECORD_SEPARATOR], :count => 2)
    @io.puts(x).should == nil
  end
end
