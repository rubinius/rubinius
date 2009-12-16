require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#inspect" do
  before :each do
    @file = File.open(tmp("inspect_spec"), "w") 
  end

  after :each do
    @file.close if !@file.closed?
    File.unlink(tmp("inspect_spec"))
  end

  it "returns a string describing a stream" do
    # don't hardcode the tmp path 
    @file.inspect.should =~ /#<File.*inspect_spec>/
    io = IO.new(@file.to_i)
    io.inspect.should =~ /<IO:[\w\s?\d]+>/
    io.close
    lambda { @file.close }.should raise_error(Errno::EBADF)
  end
end
