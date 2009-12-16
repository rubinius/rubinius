require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#to_i" do
  it "return the numeric file descriptor of the given IO object" do
    $stdout.to_i.should == 1
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.to_i }.should raise_error(IOError)
  end
end
