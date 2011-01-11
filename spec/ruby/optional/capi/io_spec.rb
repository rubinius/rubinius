require File.expand_path('../spec_helper', __FILE__)

load_extension('io')

describe "C-API IO function" do
  before :each do
    @o = CApiIOSpecs.new
  end

  describe "rb_io_write" do
    it "sends #write to the object passing the object to be written" do
      io = mock("IO")
      io.should_receive(:write).with("test").and_return(:written)
      @o.rb_io_write(io, "test").should == :written
    end
  end
end

describe "C-API IO function" do
  before :each do
    @o = CApiIOSpecs.new

    @name = tmp("c_api_io_specs")
    touch @name

    @io = new_io @name, fmode("r:utf-8")
  end

  after :each do
    @io.close unless @io.closed?
    rm_r @name
  end

  describe "rb_io_close" do
    it "closes an IO object" do
      @io.closed?.should be_false
      @o.rb_io_close(@io)
      @io.closed?.should be_true
    end
  end

  describe "rb_io_check_closed" do
    it "does not raise an exception if the IO is not closed" do
      # The MRI function is void, so we use should_not raise_error
      lambda { @o.rb_io_check_closed(@io) }.should_not raise_error
    end

    it "raises an error if the IO is closed" do
      @io.close
      lambda { @o.rb_io_check_writable(@io) }.should raise_error(IOError)
    end
  end

  describe "GetOpenFile" do
    it "allows access to the system fileno" do
      @o.GetOpenFile_fd($stdin).should == 0
      @o.GetOpenFile_fd($stdout).should == 1
      @o.GetOpenFile_fd($stderr).should == 2
      @o.GetOpenFile_fd(@io).should == @io.fileno
    end
  end
end

describe "C-API IO function" do
  before :each do
    @o = CApiIOSpecs.new
    @r_io, @w_io = IO.pipe
  end

  after :each do
    @r_io.close unless @r_io.closed?
    @w_io.close unless @w_io.closed?
  end

  describe "rb_io_check_readable" do
    it "does not raise an exception if the IO is opened for reading" do
      # The MRI function is void, so we use should_not raise_error
      lambda { @o.rb_io_check_readable(@r_io) }.should_not raise_error
    end

    it "raises an IOError if the IO is not opened for reading" do
      lambda { @o.rb_io_check_readable(@w_io) }.should raise_error(IOError)
    end
  end

  describe "rb_io_check_writable" do
    it "does not raise an exeption if the IO is opened for writing" do
      # The MRI function is void, so we use should_not raise_error
      lambda { @o.rb_io_check_writable(@w_io) }.should_not raise_error
    end

    it "raises an IOError if the IO is not opened for reading" do
      lambda { @o.rb_io_check_writable(@r_io) }.should raise_error(IOError)
    end
  end

  describe "rb_io_wait_writeable" do
    it "returns false if there is no error condition" do
      @o.rb_io_wait_writable(@w_io).should be_false
    end

    it "raises an IOError if the IO is closed" do
      @w_io.close
      lambda { @o.rb_io_wait_writable(@w_io) }.should raise_error(IOError)
    end
  end

  describe "rb_io_wait_readable" do
    it "returns false if there is no error condition" do
      @o.rb_io_wait_readable(@r_io, false).should be_false
    end

    it "raises and IOError if passed a closed stream" do
      @r_io.close
      lambda { @o.rb_io_wait_readable(@r_io, false) }.should raise_error(IOError)
    end

    it "blocks until the io is readable and returns true" do
      @o.instance_variable_set :@write_data, false
      thr = Thread.new do
        sleep 0.1 until @o.instance_variable_get(:@write_data)
        @w_io.write "rb_io_wait_readable"
      end

      Thread.pass until thr.alive?

      @o.rb_io_wait_readable(@r_io, true).should be_true
      @o.instance_variable_get(:@read_data).should == "rb_io_wait_re"
    end
  end
end
