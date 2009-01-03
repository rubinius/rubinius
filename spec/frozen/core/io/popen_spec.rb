require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO::popen" do
  # NOTE: cause Errno::EBADF on 1.8.6
  #ruby_bug "#", "1.8.6" do
    it "reads from a read-only pipe" do
      IO.popen("echo foo", "r") do |io|
        io.read.should == "foo\n"

        lambda { io.write('foo').should }.should \
          raise_error(IOError, 'not opened for writing')
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

      it "writes to a write-only pipe" do
        begin
          tmp_file = tmp "IO_popen_spec_#{$$}"

          data = IO.popen "cat > #{tmp_file}", 'w' do |io|
            io.write 'bar'

            lambda { io.read.should }.should \
              raise_error(IOError, 'not opened for reading')
          end

          File.read(tmp_file).should == 'bar'

        ensure
          File.unlink tmp_file if File.exist? tmp_file
        end
      end
    end

    it "allows the io to be closed inside the block" do
      io = IO.popen('yes', 'r') do |io|
        io.close

        io.closed?.should == true

        io
      end

      io.closed?.should == true
    end
  #end
end

