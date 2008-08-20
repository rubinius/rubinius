require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require 'socket'

describe "Socket#for_fd given a file descriptor" do
  it "adopts that descriptor into a new Socket object" do
    server = TCPServer.new("0.0.0.0", 50000)
    client = TCPSocket.open("0.0.0.0", 50000)
    host = server.accept_nonblock

    new_sock = Socket.for_fd(client.fileno)
    
    # TODO: RSpec uses #send internally when calling #should == something
    # but as Socket#send does not do what RSpex expects it to do,
    # it fails horriby. RSpec should be using #__send__ 
    new_sock.should_not be_nil
    new_sock.class.should == Socket
    new_sock.fileno.should == client.fileno

    new_sock.write("foo")
    client.write("bar")
    host.read(3).should == "foo"
    host.read(3).should == "bar"
  end

  it "raises EBADF for a bad descriptor" do
    lambda { Socket.for_fd(9999999) }.should raise_error(Errno::EBADF)
  end
end

