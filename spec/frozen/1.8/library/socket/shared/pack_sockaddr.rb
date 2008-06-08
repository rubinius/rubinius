shared :pack_sockaddr_in do |cmd|
  describe "Socket##{cmd}" do

    it "packs and unpacks" do
      sockaddr_in = Socket.pack_sockaddr_in 0, ''
      Socket.unpack_sockaddr_in(sockaddr_in).should == [0, '0.0.0.0']

      sockaddr_in = Socket.pack_sockaddr_in 80, '127.0.0.1'
      Socket.unpack_sockaddr_in(sockaddr_in).should == [80, '127.0.0.1']
    end
  end
end

shared :pack_sockaddr_un do |cmd|
  not_supported_on :jruby, :windows do
    describe "Socket##{cmd}" do
      it "packs and unpacks" do
        sockaddr_un = Socket.pack_sockaddr_un '/tmp/s'
        Socket.unpack_sockaddr_un(sockaddr_un).should == '/tmp/s'
      end
    end
  end
end

