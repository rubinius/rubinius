shared :pack_sockaddr_in do |cmd|
  describe "Socket##{cmd}" do

    compliant_on :rubinius do
      it "properly encodes the sin_family portion of sockaddr_in struct" do
        SocketSpecs.sockaddr_in(0, "")[:sin_family].should == Socket::AF_INET
      end

      it "properly encodes the sin_port portion of sockaddr_in struct" do
        SocketSpecs.sockaddr_in(0, "")[:sin_port].should == 0
        SocketSpecs.sockaddr_in(1, "")[:sin_port].should == 256    # possibly different on big endian machines
        SocketSpecs.sockaddr_in(80, "")[:sin_port].should == 20480 # possibly different on big endian machines
      end 

      it "properly encodes the sin_addr portion of sockaddr_in struct" do
        SocketSpecs.sockaddr_in(0, "127.0.0.1")[:sin_addr].should == 16777343
        SocketSpecs.sockaddr_in(0, "0.0.0.0")[:sin_addr].should == 0
      end

      not_compliant_on :rubinius do
        it "encodes an empty sin_addr in sockaddr_in struct as 0" do
          SocketSpecs.sockaddr_in(0, "")[:sin_addr].should == 0
        end
      end
    end
  end
end


shared :pack_sockaddr_un do |cmd|
  if Socket.const_defined?(:AF_UNIX)
    describe "Socket##{cmd}" do
      it "encodes the unix socket type into the string" do
        Socket.sockaddr_un("/tmp/s").unpack("ccA104")[2].should == "/tmp/s"
      end
    end
  end
end