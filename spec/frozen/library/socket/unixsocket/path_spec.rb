require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "UNIXSocket#path" do

  platform_is_not :windows do
    before :all do
      @path = SocketSpecs.socket_path
      @server = UNIXServer.open(@path)
      @client = UNIXSocket.open(@path)
    end

    after :all do
      @client.close
      @server.close
      File.unlink(@path) if File.exists?(@path)
    end

    it "returns the path of the socket if it's a server" do
      @server.path.should == @path
    end

    it "returns an empty string for path if it's a client" do
      @client.path.should == ""
    end
  end

end
