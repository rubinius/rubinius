require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

@dir_open = shared "Dir.open" do |cmd|
  it "returns a Dir instance representing the specified directory" do
    dir = Dir.send(cmd, mock_dir)
    dir.class.should == Dir
    dir.close
  end
  
  it "raises SystemCallError if the directory does not exist" do
    should_raise(SystemCallError) { Dir.send cmd, nonexistent }
  end
  
  it "takes a block which yields the Dir instance and closes it after" do
    # This is a bit convoluted but we are trying to ensure the file gets closed.
    # To do that, we peek to see what the next FD number is and then probe that
    # to see whether it has been closed.
    peek = IO.sysopen mock_dir
    File.for_fd(peek).close

    Dir.send(cmd, mock_dir) { }
    should_raise(SystemCallError) { File.for_fd peek }
  end
  
  it "returns the value of the block if a block is passed" do
    Dir.open(mock_dir) {|dir| dir.class}.should == Dir
  end
end

describe "Dir.open" do
  it_behaves_like @dir_open, :open
end
