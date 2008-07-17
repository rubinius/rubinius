describe :stringio_readchar, :shared => true do
  before(:each) do
    @io = StringIO.new("example")
  end

  it "reads the next 8-bit byte from self's current position" do
    @io.send(@method).should == ?e

    @io.pos = 4
    @io.send(@method).should == ?p
  end

  it "correctly updates the current position" do
    @io.send(@method)
    @io.pos.should == 1

    @io.send(@method)
    @io.pos.should == 2
  end

  it "raises an EOFError when self is at the end" do
    @io.pos = 7
    lambda { @io.send(@method) }.should raise_error(EOFError)
  end
end

describe :stringio_readchar_not_readable, :shared => true do
  it "raises an IOError" do
    io = StringIO.new("a b c d e", "w")
    lambda { io.send(@method) }.should raise_error(IOError)

    io = StringIO.new("a b c d e")
    io.close_read
    lambda { io.send(@method) }.should raise_error(IOError)
  end
end
