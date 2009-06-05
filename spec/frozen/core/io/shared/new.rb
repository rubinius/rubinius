require File.dirname(__FILE__) + '/../fixtures/classes'

describe :io_new, :shared => true do
  before :all do
    @filename = tmp("rubinius-spec-io-new-#{$$}.txt")
  end

  after :all do
    File.unlink @filename
  end

  before :each do
    @file = File.open @filename, "w"
  end

  after :each do
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "returns a new IO object" do
    begin
      io = IO.send(@method, @file.fileno, 'w')
    ensure
      io.close
    end
    io.class.should == IO
  end

  it "takes an Integer or #to_int argument as the descriptor to open" do
    o = mock('descriptor')
    o.should_receive(:to_int).any_number_of_times.and_return(@file.fileno)

    begin
      io = IO.send(@method, @file.fileno, 'w')
      io.fileno.should == @file.fileno

      io2 = IO.send(@method, o, 'w')
      io2.fileno.should == @file.fileno
    ensure
      io.close unless io.closed? rescue Errno::EBADF
      io2.close unless io2.closed? rescue Errno::EBADF
    end
  end

  it "associates new IO with the old descriptor so each IO directly affects the other" do
    io = IO.send @method, @file.fileno, 'w'

    @file.syswrite "Hello "
    @file.closed?.should == false

    io.close
    io.closed?.should == true

    # Using #syswrite to force no Ruby buffering which could mask this error
    lambda { @file.syswrite "there\n" }.should raise_error(Errno::EBADF)
  end

  it "raises TypeError if not given an Integer or #to_int" do
    lambda { IO.send(@method, nil, 'r') }.should raise_error(TypeError)
    lambda { IO.send(@method, Object.new, 'r') }.should raise_error(TypeError)
  end

  it "raises EBADF if the file descriptor given is not a valid and open one" do
    lambda { IO.send(@method, -2, 'r') }.should raise_error(Errno::EBADF)

    fd = @file.fileno
    @file.close
    lambda { IO.send(@method, fd, 'w') }.should raise_error(Errno::EBADF)
  end

  ruby_version_is "" ... "1.9" do
    it "raises EINVAL if mode is not compatible with the descriptor's current mode" do
      lambda { IO.send(@method, @file.fileno, 'r') }.
        should raise_error(Errno::EINVAL)
      lambda { io = IO.send(@method, @file.fileno, 'w'); io.close }.
        should_not raise_error
    end
  end

  ruby_version_is "1.9" do
    it "does not raise EINVAL even if mode is not compatible with the descriptor's current mode" do
      lambda { IO.send(@method, @file.fileno, 'r') }.
        should_not raise_error(Errno::EINVAL)
      lambda { io = IO.send(@method, @file.fileno, 'w'); io.close }.
        should_not raise_error
    end
  end

  it "raises IOError on closed stream" do
    lambda { IO.new(IOSpecs.closed_file.fileno, 'w') }.should raise_error(IOError)
  end

  it "does not close the stream automatically if given a block" do
    begin
      io = IO.new(@file.fileno, 'w') {|f| puts f.read }
      io.closed?.should == false
      @file.closed?.should == false
    ensure
      io.close
    end
  end

  it "emits a warning if given a block" do
    lambda {
      io = IO.new(@file.fileno, 'w') {|io| puts io.read }
      io.close
    }.should complain(/IO::new.*does not take block.*IO::open.*instead/)
  end

  it "accepts only one argument" do
    # By default, IO.new without an arg assumes RO
    @file.close
    io = ""
    @file = File.open @filename, 'r'
    lambda {
      io = IO.new(@file.fileno)
    }.should_not raise_error()

    io.close
  end

  ruby_version_is "" ... "1.9" do
    it "cannot open an IO with incompatible flags" do
      lambda { IO.new(@file.fileno, "r") }.should raise_error
    end
  end

  ruby_version_is "1.9" do
    it "can open an IO with incompatible flags" do
      lambda { IO.new(@file.fileno, "r") }.should_not raise_error
    end
  end
end
