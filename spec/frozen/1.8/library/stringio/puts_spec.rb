require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#puts" do
  before(:each) do
    @io = StringIO.new('')
  end

  after(:each) do
    $/ = "\n"
  end

  it "writes a newline after objects that do not end in newlines" do
    @io.puts(5).should == nil
    @io.string.should == "5\n"
  end

  it "does not write a newline after objects that end in newlines" do
    @io.puts("5\n").should == nil
    @io.string.should == "5\n"
  end
  
  it "calls to_s before writing non-string objects" do
    object = mock('hola')
    object.should_receive(:to_s).and_return("hola")
    
    @io.puts(object).should == nil
    @io.string.should == "hola\n"
  end
  
  it "writes each arg if given several" do
    @io.puts(1, "two", 3).should == nil
    @io.string.should == "1\ntwo\n3\n"
  end
  
  it "flattens a nested array before writing it" do
    @io.puts([1, 2, [3, [4, [5]]]]).should == nil
    @io.string.should == "1\n2\n3\n4\n5\n"
  end
  
  it "writes [...] for a recursive array arg" do
    x = []
    x << 2 << x
    @io.puts(x).should == nil
    @io.string.should == "2\n[...]\n"
  end
  
  it "writes a newline after objects that do not end in newlines" do
    @io.puts(5).should == nil
    @io.string.should == "5\n"
  end
  
  it "does not write a newline after objects that end in newlines" do
    @io.puts("5\n").should == nil
    @io.string.should == "5\n"
  end

  it "ignores the $/ separator global" do
    $/ = ":"
    @io.puts(5,6)
    @io.string.should == "5\n6\n"
  end
end
