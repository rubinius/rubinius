require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO::popen" do
  # NOTE: cause Errno::EBADF on 1.8.6
  #ruby_bug "#", "1.8.6" do
    it "reads from a read-only pipe" do
      IO.popen("echo foo", "r") do |io|
        io.read.should == "foo\n"
      end
    end

    platform_is_not :windows do
      it "reads and writes to a read/write pipe" do
        data = IO.popen("cat", "r+") do |io|
          io.write("bar")
          io.read 3
        end
      
        data.should == "bar"
      end
    end

    it "allows the io to be closed inside the block" do
      io = IO.popen('yes', 'r') do |io|
        io.close
        io
      end
    
      io.closed?.should == true
    end
  #end
end

describe "IO::popen" do
  it "needs to be reviewed for spec completeness" do
  end
end
