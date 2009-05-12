require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require 'socket'

describe "Socket#for_fd given a file descriptor" do
  it "adopts that descriptor into a new Socket object" do
    server = TCPServer.new("0.0.0.0", SocketSpecs.port)
    client = TCPSocket.open("0.0.0.0", SocketSpecs.port)

    new_sock = Socket.for_fd(client.fileno)
    
    # TODO: RSpec uses #send internally when calling #should == something
    # but as Socket#send does not do what RSpex expects it to do,
    # it fails horriby. RSpec should be using #__send__ 
    new_sock.should_not be_nil
    new_sock.class.should == Socket
    new_sock.fileno.should == client.fileno

    new_sock.write("foo")
    client.write("bar")
    host = server.accept
    host.read(3).should == "foo"
    host.read(3).should == "bar"

    server.close unless server.closed?
    client.close unless server.closed?
    new_sock.close unless new_sock.closed?
  end

  it "raises error for a bad descriptor" do
    lambda { Socket.for_fd(9999999) }.should raise_error
  end
end

