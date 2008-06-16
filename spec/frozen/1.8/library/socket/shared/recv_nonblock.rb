shared :recv_nonblock do |klass, cmd|
  not_supported_on :jruby do
    describe "Socket::#{klass}.#{cmd}" do
      before :each do
        @s1 = Socket.new(Socket::AF_INET, Socket::SOCK_DGRAM, 0)
        @s2 = Socket.new(Socket::AF_INET, Socket::SOCK_DGRAM, 0)
      end

      after :each do
        @s1.close if @s1
        @s2.close if @s2
      end

      it "raises EAGAIN if there's no data available" do
        @s1.bind(Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1"))
        lambda { @s1.recv_nonblock(5)}.should raise_error(Errno::EAGAIN)
        lambda { @s2.recv_nonblock(5)}.should raise_error(Errno::EAGAIN)
      end

      it "receives data after it's ready" do
        @s1.bind(Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1"))
        @s2.send("aaa", 0, @s1.getsockname)
        IO.select([@s1], nil, nil, 2)
        @s1.recv_nonblock(3).should == "aaa"
      end

      it "does not block if there's no data available" do
        @s1.bind(Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1"))
        @s2.send("a", 0, @s1.getsockname)
        IO.select([@s1], nil, nil, 2)
        @s1.recv_nonblock(1).should == "a"
        lambda { @s1.recv_nonblock(1)}.should raise_error(Errno::EAGAIN)       
      end
    end  
  end  
end
