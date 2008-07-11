require File.dirname(__FILE__) + '/../spec_helper'

describe IOStub do
  before :each do
    @out = IOStub.new
    @sep = $\
  end

  after :each do
    $\ = @sep
  end

  it "provides a write method" do
    @out.write "this"
    @out.should == "this"
  end

  it "concatenates the arguments sent to write" do
    @out.write "flim ", "flam"
    @out.should == "flim flam"
  end

  it "provides a print method that appends the default separator" do
    $\ = " [newline] "
    @out.print "hello"
    @out.print "world"
    @out.should == "hello [newline] world [newline] "
  end

  it "provides a puts method that appends the default separator" do
    @out.puts "hello", 1, 2, 3
    @out.should == "hello\n1\n2\n3\n"
  end

  it "provides a flush method that does nothing and returns self" do
    @out.flush.should == @out
  end
end
