require File.expand_path('../spec_helper', __FILE__)

load_extension('io')

describe "C-API IO function" do
  before :each do
    @o = CApiIOSpecs.new
    @read_end, @write_end = IO.pipe
  end

  describe "rb_io_write" do
    it "sends #write to the object passing the object to be written" do
      io = mock("IO")
      io.should_receive(:write).with("test").and_return(:written)
      @o.rb_io_write(io, "test").should == :written
    end
  end

  describe "rb_io_close" do
    it "closes an IO object" do
      io = File.open(__FILE__, "r")
      io.closed?.should be_false

      @o.rb_io_close(io)

      io.closed?.should be_true
    end
  end

  describe "rb_io_check_readable" do
    it "raises an IOError if the io isn't opened for reading" do
      lambda { @o.rb_io_check_readable(@write_end) }.should raise_error(IOError)
    end

    it "raises no error if the io is opened for reading" do
      lambda { @o.rb_io_check_readable(@read_end) }.should_not raise_error(IOError)
    end
  end

  describe "rb_io_check_writable" do
    it "raises no error if the io is opened for writing" do
      lambda { @o.rb_io_check_writable(@write_end) }.should_not raise_error(IOError)
    end

    it "raises an IOError if the io is not opened for reading" do
      lambda { @o.rb_io_check_writable(@read_end) }.should raise_error(IOError)
    end
  end

  describe "rb_io_check_closed" do
    it "raises an error only if the io is closed" do
      lambda { @o.rb_io_check_writable(@write_end) }.should_not raise_error(IOError)
      @write_end.close
      lambda { @o.rb_io_check_writable(@write_end) }.should raise_error(IOError)
    end
  end

  describe "rb_io_wait_writeable" do
    it "raises and IOError if passed a closed stream" do
      @write_end.close
      lambda { @o.rb_io_wait_writable(@write_end) }.should raise_error(IOError)
    end
  end

  describe "rb_io_wait_readable" do
    it "blocks until the io is readable" do
      @write_end.write("foo")
      @o.rb_io_wait_readable(@read_end).should == nil # read went fine
      Thread.new(@write_end) {|w| sleep 0.1; w.write("foo") }
      @o.rb_io_wait_readable(@read_end).should == true # no data immediately available, but incoming
      @write_end.close
      @read_end.read
      @o.rb_io_wait_readable(@read_end).should == false # no data available and input closed
    end

    it "raises and IOError if passed a closed stream" do
      @read_end.close
      lambda { @o.rb_io_wait_readable(@read_end) }.should raise_error(IOError)
    end
  end

  describe "GetOpenFile" do
    it "allows access to the system fileno" do
      @o.GetOpenFile_fd($stdin).should == 0
      @o.GetOpenFile_fd($stdout).should == 1
      @o.GetOpenFile_fd($stderr).should == 2
      @o.GetOpenFile_fd(@read_end).should == @read_end.fileno
    end
  end
end
