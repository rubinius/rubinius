require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#printf" do
  before :each do
    @io = IO.new STDOUT.fileno, 'w'
  end

  fails_on :jruby do
    it "writes the #sprintf formatted string to the file descriptor" do
      lambda {
        @io.printf "%s\n", "look ma, no hands"
      }.should output_to_fd("look ma, no hands\n", @io)
    end
  end
end
