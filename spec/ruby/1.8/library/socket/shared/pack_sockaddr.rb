shared :pack_sockaddr_in do |cmd|
  describe "Socket##{cmd}" do

    it "properly encodes the sin_family portion of sockaddr_in struct" do
      Socket.sockaddr_in(80, "").unpack("sslc")[0].should == 2
    end

    it "properly encodes the sin_port portion of sockaddr_in struct" do
      Socket.sockaddr_in(0, "").unpack("sslc")[1].should == 0
      Socket.sockaddr_in(1, "").unpack("sslc")[1].should == 256    # possibly different on big endian machines
      Socket.sockaddr_in(80, "").unpack("sslc")[1].should == 20480 # possibly different on big endian machines
    end 

    it "properly encodes the sin_addr portion of sockaddr_in struct" do
      Socket.sockaddr_in(0, "127.0.0.1").unpack("sslc")[2].should == 16777343
      Socket.sockaddr_in(0, "0.0.0.0").unpack("sslc")[2].should == 0
    end
   
    not_compliant_on :rubinius do
      it "encodes an empty sin_addr in sockaddr_in struct as 0" do
        Socket.sockaddr_in(80, "").unpack("sslc")[2].should == 0
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