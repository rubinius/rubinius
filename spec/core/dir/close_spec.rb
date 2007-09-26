require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir#close" do
  it "closes the stream and fd and returns nil" do
    # This is a bit convoluted but we are trying to ensure the file gets closed.
    # To do that, we peek to see what the next FD number is and then probe that
    # to see whether it has been closed.
    peek = IO.sysopen mock_dir
    File.for_fd(peek).close

    dir = Dir.open mock_dir
    File.for_fd(peek).close                   # Should be open here

    dir.close.should == nil
    should_raise(SystemCallError) { File.for_fd(peek).close }  # And closed here
  end
end

@dir_closed = shared "Dir closed" do |cmd|
  describe "Dir##{cmd}" do
    it "raises an IOError when called on a closed Dir instance" do
      should_raise(IOError) do
        dir = Dir.open mock_dir
        dir.close
        dir.send cmd
      end
    end
  end
end

describe "Dir#close" do
  it_behaves_like @dir_closed, :close
end
