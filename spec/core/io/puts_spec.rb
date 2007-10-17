require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: need to find a better way to test this. Too fragile to set expectations
# to each write call. Only care that all the characters are sent not the number
# or write calls. Also, these tests do not make sure the ordering of the write calls
# are correct.
describe "IO#puts" do
  before(:each) do
    @io = IO.new(2)
  end
  
  it "writes just a newline when given no args" do
    @io.should_receive(:write, :with => ["\n"])
    @io.puts.should == nil
  end
  
  it "writes nil with a newline when given nil as an arg" do
    @io.should_receive(:write, :with => ["nil"])
    @io.should_receive(:write, :with => ["\n"])
    @io.puts(nil).should == nil
  end
  
  it "calls to_s before writing non-string objects" do
    object = Object.new
    object.should_receive(:to_s, :returning => "hola")
    
    @io.should_receive(:write, :with => ["hola"])
    @io.should_receive(:write, :with => ["\n"])
    @io.puts(object).should == nil
  end
  
  it "writes each arg if given several" do
    @io.should_receive(:write, :with => ["1"])
    @io.should_receive(:write, :with => ["two"])
    @io.should_receive(:write, :with => ["3"])
    @io.should_receive(:write, :with => ["\n"], :count => 3)
    @io.puts(1, "two", 3).should == nil
  end
  
  it "flattens a nested array before writing it" do
    @io.should_receive(:write, :with => ["1"])
    @io.should_receive(:write, :with => ["2"])
    @io.should_receive(:write, :with => ["3"])
    @io.should_receive(:write, :with => ["\n"], :count => 3)
    @io.puts([1, 2, [3]]).should == nil
  end
  
  it "writes [...] for a recursive array arg" do
    x = []
    x << 2 << x
    @io.should_receive(:write, :with => ["2"])
    @io.should_receive(:write, :with => ["[...]"])
    @io.should_receive(:write, :with => ["\n"], :count => 2)
    @io.puts(x).should == nil
  end
  
  it "writes a newline after objects that do not end in newlines" do
    @io.should_receive(:write, :with => ["5"])
    @io.should_receive(:write, :with => ["\n"])
    @io.puts(5).should == nil
  end

  it "does not write a newline after objects that end in newlines" do
    @io.should_receive(:write, :with => ["5\n"])
    @io.puts("5\n").should == nil
  end
  
  it "ignores the $/ separator global" do
    $/ = ":"
    @io.should_receive(:write, :with => ["5"])
    @io.should_receive(:write, :with => ["\n"])
    @io.puts(5).should == nil
    $/ = "\n"
  end
end
