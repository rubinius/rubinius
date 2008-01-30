shared :io_new do |cmd|
  describe "IO##{cmd}" do
    before :all do
      @filename = "/tmp/rubinius-spec-io-new-#{$$}.txt"
    end

    after :all do
      File.unlink @filename
    end

    before :each do
      @file = File.open @filename, "w"
    end

    after :each do
      # This should normally NOT be rescued
      @file.close unless @file.closed? rescue nil
    end

    it "returns a new IO object" do
      IO.send(cmd, @file.fileno, 'w').class.should == IO
    end

    it "takes an Integer or #to_int argument as the descriptor to open" do
      o = mock('descriptor')
      o.should_receive(:to_int).any_number_of_times.and_return(@file.fileno)

      IO.send(cmd, @file.fileno, 'w').fileno.should == @file.fileno
      IO.send(cmd, o, 'w').fileno.should == @file.fileno
    end

    it "associates new IO with the old descriptor so each IO directly affects the other" do
      io = IO.send cmd, @file.fileno, 'w'
      io.fileno.should == @file.fileno

      @file.syswrite "Hello "
      @file.closed?.should == false

      io.close
      io.closed?.should == true

      # Using #syswrite to force no Ruby buffering which could mask this error
      lambda { @file.syswrite "there\n" }.should raise_error(Errno::EBADF)
    end

    it "raises TypeError if not given an Integer or #to_int" do
      lambda { IO.send(cmd, nil, 'r') }.should raise_error(TypeError)
      lambda { IO.send(cmd, Object.new, 'r') }.should raise_error(TypeError)
    end

    it "raises EBADF if the file descriptor given is not a valid and open one" do
      lambda { IO.send(cmd, -2, 'r') }.should raise_error(Errno::EBADF)

      fd = @file.fileno
      @file.close
      lambda { IO.send(cmd, fd, 'w') }.should raise_error(Errno::EBADF)
    end

    it "raises EINVAL if mode is not compatible with the descriptor's current mode" do
      lambda { IO.send(cmd, @file.fileno, 'r') }.should raise_error(Errno::EINVAL)
      lambda { IO.send(cmd, @file.fileno, 'w') }.should_not raise_error
    end
  end
end
