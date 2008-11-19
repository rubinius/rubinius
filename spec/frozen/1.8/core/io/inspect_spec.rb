require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#inspect" do
  before :each do
    @file = File.open(tmp("inspect_spec"), "w") 
  end

  after :each do
    @file.close
    File.unlink(tmp("inspect_spec"))
  end

  it "returns a string describing a stream" do
    # don't hardcode the tmp path 
    @file.inspect.should =~ /#<File.*inspect_spec>/
    IO.new(@file.to_i).inspect.should =~ /<IO:[\w\d]+>/
  end
end
