require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#to_io" do
  it "returns self for open stream" do
    io = IO.new(2, 'w')
    io.to_io.should == io

    File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r') { |io|
      io.to_io.should == io
    }
  end

  it "returns self for closed stream" do
    io = IOSpecs.closed_file
    io.to_io.should == io
  end
end
