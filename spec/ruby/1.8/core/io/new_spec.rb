require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.new" do
  before :all do
    @filename = "/tmp/rubinius-spec-io-new-#{$$}.txt"
  end

  after :all do
    File.unlink @filename
    @io, @var = nil, nil
  end

  before :each do
    @file = File.open @filename, "w"
  end

  after :each do
    # This should normally NOT be rescued
    @file.close unless @file.closed? rescue nil
  end

  it "returns a new IO object" do
    IO.new(@file.fileno, 'w').class.should == IO
  end

  it "takes an Integer or #to_int argument as the descriptor to open" do
    o = mock('descriptor')
    o.should_receive(:to_int).any_number_of_times.and_return(@file.fileno)

    IO.new(@file.fileno, 'w').fileno.should == @file.fileno
    IO.new(o, 'w').fileno.should == @file.fileno
  end

  it "associates new IO with the old descriptor so each IO directly affects the other" do
    io = IO.new @file.fileno, 'w'
    io.fileno.should == @file.fileno

    @file.syswrite "Hello "
    @file.closed?.should == false

    io.close
    io.closed?.should == true

    # Using #syswrite to force no Ruby buffering which could mask this error
    lambda { @file.syswrite "there\n" }.should raise_error(Errno::EBADF)
  end

  it "raises TypeError if not given an Integer or #to_int" do
    lambda { IO.new(nil, 'r') }.should raise_error(TypeError)
    lambda { IO.new(Object.new, 'r') }.should raise_error(TypeError)
  end

  it "raises EBADF if the file descriptor given is not a valid and open one" do
    lambda { IO.new(-2, 'r') }.should raise_error(Errno::EBADF)

    fd = @file.fileno
    @file.close
    lambda { IO.new(fd, 'w') }.should raise_error(Errno::EBADF)
  end

  it "raises EINVAL if mode is not compatible with the descriptor's current mode" do
    lambda { IO.new(@file.fileno, 'r') }.should raise_error(Errno::EINVAL)
    lambda { IO.new(@file.fileno, 'w') }.should_not raise_error
  end

  it "does not execute a block if given one" do
    l = lambda { IO.new(@file.fileno, 'w') {|io| raise Exception, "N-uh" } }
    l.should_not raise_error(Exception, "N-uh")
  end

  it "does not close the stream automatically if given a block" do
    io = IO.new(@file.fileno, 'w') {|f| puts f.read }

    io.closed?.should == false
    @file.closed?.should == false
  end

  it "emits a warning if given a block" do
    lambda {
      IO.new(@file.fileno, 'w') {|io| puts io.read }
    }.should complain(/IO::new.*does not take block.*IO::open.*instead/)
  end
end
