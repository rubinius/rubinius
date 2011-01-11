require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

ruby_version_is ""..."1.9" do
  describe "BasicSocket.do_not_reverse_lookup" do
    before :all do
      @do_not_reverse_lookup = BasicSocket.do_not_reverse_lookup
    end
    
    before(:each) do
      @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
      @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    end
    
    after(:each) do
      @socket.close unless @socket.closed?
      @server.close unless @server.closed?
    end
    
    after :all do
      BasicSocket.do_not_reverse_lookup = @do_not_reverse_lookup
    end
    
    it "defaults to false" do
      BasicSocket.do_not_reverse_lookup.should be_false
    end

    it "causes 'peeraddr' to avoid name lookups" do
      BasicSocket.do_not_reverse_lookup = true
      @socket.peeraddr.should == ["AF_INET", SocketSpecs.port, "127.0.0.1", "127.0.0.1"]
    end

    it "looks for hostnames when set to false" do
      BasicSocket.do_not_reverse_lookup = false
      @socket.peeraddr[2].should == SocketSpecs.hostname
    end

    it "looks for numeric addresses when set to true" do
      BasicSocket.do_not_reverse_lookup = true
      @socket.peeraddr[2].should == "127.0.0.1"
    end
  end
end

ruby_version_is "1.9" do
  describe "BasicSocket.do_not_reverse_lookup" do
    before(:each) do
      @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
      @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    end
    
    after(:each) do
      @server.close unless @server.closed?
      @socket.close unless @socket.closed?
    end
    
    it "defaults to true" do
      BasicSocket.do_not_reverse_lookup.should be_true
    end

    it "causes 'peeraddr' to avoid name lookups" do
      @socket.do_not_reverse_lookup = true
      BasicSocket.do_not_reverse_lookup = true
      @socket.peeraddr.should == ["AF_INET", SocketSpecs.port, "127.0.0.1", "127.0.0.1"]
    end

    it "looks for hostnames when set to false" do
      @socket.do_not_reverse_lookup = false
      BasicSocket.do_not_reverse_lookup = false
      @socket.peeraddr[2].should == SocketSpecs.hostname
    end

    it "looks for numeric addresses when set to true" do
      @socket.do_not_reverse_lookup = true
      BasicSocket.do_not_reverse_lookup = true
      @socket.peeraddr[2].should == "127.0.0.1"
    end
  end
end
