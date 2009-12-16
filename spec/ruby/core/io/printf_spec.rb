require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#printf" do
  before :each do
    @io = IO.new STDOUT.fileno, 'w'
  end

  it "writes the #sprintf formatted string to the file descriptor" do
    lambda {
      @io.printf "%s\n", "look ma, no hands"
    }.should output_to_fd("look ma, no hands\n", @io)
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.printf("stuff") }.should raise_error(IOError)
  end
end
