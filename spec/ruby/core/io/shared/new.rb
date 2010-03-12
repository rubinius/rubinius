require File.expand_path('../../fixtures/classes', __FILE__)

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
    @file.close unless @file.closed?
  end

  # It's impossible for IO.new to interact with other code because it
  # can easily cause EBADF to be randomly raised from any IO method.
  # Because of this, I (EMP) do not believe they can even be run. If
  # you, the reader, want to run them, remove this quarantine guard
  # manually, but DO NOT COMMIT THE REMOVAL.
  #
  # If you wish to further discuss this policy, please contact me.
  # Thanks for listening.
  #
  quarantine! do

  it "takes an Integer argument as the descriptor to open" do
    o = mock('descriptor')
    o.should_receive(:to_int).any_number_of_times.and_return(@file.fileno)

    begin
      io = IO.send(@method, o, 'w')
      io.fileno.should == @file.fileno
    ensure
      io.close
    end

    o = mock("not to_i")

    lambda { IO.send(@method, o, "w") }.should raise_error(TypeError)
  end

  it "returns a new IO object" do
    begin
      io = IO.send(@method, @file.fileno, 'w')
    ensure
      io.close
    end
    io.should be_an_instance_of(IO)
  end

  it "raises EBADF if the file descriptor given is not a valid and open one" do
    lambda { IO.send(@method, -2, 'r') }.should raise_error(Errno::EBADF)
  end

  # (1.9 behaviour verified as correct in bug #1582)
  it "raises EINVAL if mode is not compatible with the descriptor's current mode" do
    lambda { IO.send(@method, @file.fileno, 'r') }.
      should raise_error(Errno::EINVAL)
    lambda { io = IO.send(@method, @file.fileno, 'w'); io.close }.
      should_not raise_error
  end

  it "raises IOError on closed stream" do
    lambda { IO.new(IOSpecs.closed_io.fileno, 'w') }.should raise_error(IOError)
  end

  # (1.9 behaviour verified as correct in bug #1582)
  it "cannot open an IO with incompatible flags" do
    lambda { IO.new(@file.fileno, "r") }.should raise_error(Errno::EINVAL)
  end

  end
end
