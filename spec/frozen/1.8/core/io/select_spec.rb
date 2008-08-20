require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.select" do
  before :each do
    @rd, @wr = IO.pipe
  end

  after :each do
    @rd.close unless @rd.closed?
    @wr.close unless @wr.closed?
  end

  it "invokes to_io on supplied objects that are not IO" do
    # make some data available
    @wr.write("foobar")

    obj = mock("read_io")
    obj.should_receive(:to_io).at_least(1).and_return(@rd)
    IO.select([obj]).should == [[obj], [], []]

    obj = mock("write_io")
    obj.should_receive(:to_io).at_least(1).and_return(@wr)
    IO.select(nil, [obj]).should == [[], [obj], []]
  end

  it "raises TypeError if supplied objects are not IO" do
    lambda { IO.select([Object.new]) }.should raise_error(TypeError)
    lambda { IO.select(nil, [Object.new]) }.should raise_error(TypeError)

    obj = mock("io")
    obj.should_receive(:to_io).any_number_of_times.and_return(nil)

    lambda { IO.select([obj]) }.should raise_error(TypeError)
    lambda { IO.select(nil, [obj]) }.should raise_error(TypeError)
  end

  it "raises TypeError if the specified timeout value is not Numeric" do
    lambda { IO.select([@rd], nil, nil, Object.new) }.should raise_error(TypeError)
  end

  it "raises TypeError if the first three arguments are not Arrays" do
    lambda { IO.select(Object.new)}.should raise_error(TypeError)
    lambda { IO.select(nil, Object.new)}.should raise_error(TypeError)
    lambda { IO.select(nil, nil, Object.new)}.should raise_error(TypeError)
  end

  it "needs to be reviewed for spec completeness"

end
