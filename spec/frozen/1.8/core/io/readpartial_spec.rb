require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readpartial" do
  before :each do
    @rd, @wr = IO.pipe
  end

  after :each do
    @rd.close unless @rd.closed?
    @wr.close unless @wr.closed?
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.readpartial(10) }.should raise_error(IOError)

    @rd.close
    lambda { @rd.readpartial(10) }.should raise_error(IOError)
  end

  it "reads at most the specified number of bytes" do
    @wr.write("foobar")

    # buffered read
    @rd.read(1).should == 'f'
    # return only specified number, not the whole buffer
    @rd.readpartial(1).should == "o"
  end

  it "reads after ungetc with data in the buffer" do
    @wr.write("foobar")
    c = @rd.getc
    @rd.ungetc(c)
    @rd.readpartial(3).should == "foo"
    @rd.readpartial(3).should == "bar"
  end

  it "reads after ungetc without data in the buffer" do
    @wr.write("f")
    c = @rd.getc
    @rd.ungetc(c)
    @rd.readpartial(2).should == "f"

    # now, also check that the ungot char is cleared and
    # not returned again
    @wr.write("b")
    @rd.readpartial(2).should == "b"
  end

  it "raises ArgumentError if the negative argument is provided" do
    lambda { @rd.readpartial(-1) }.should raise_error(ArgumentError, /negative/)
  end

  it "immediately returns an empty string if the length argument is 0" do
    @rd.readpartial(0).should == ""
  end

end
