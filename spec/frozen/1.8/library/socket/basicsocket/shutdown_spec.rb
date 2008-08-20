require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#shutdown" do

  before :each do
    @s1 = TCPServer.new(SocketSpecs.port)    
    @s2 = TCPSocket.new("127.0.0.1", SocketSpecs.port)    
  end

  after :each do
    @s1.close unless @s1.closed?
    @s2.close unless @s2.closed?
  end

  it "closes the writing end of the socket when arg is 1" do
    @s2.send("foo", 0).should == 3
    @s2.shutdown(1)
    lambda { @s2.send("foo", 0) }.should raise_error
  end

#   ruby_version_is "" ... "1.9" do
#     ruby_bug "#", "1.8" do
#       it "closes both ends of the socket when arg is 2" do
#         @s2.shutdown(2)
#         lambda { @s2.send("foo", 0) }.should raise_error
#         lambda { @s2.recv(5) }.should raise_error
#       end
#     end
#   end
#   it "raises an error if arg is not 0, 1 or 2" do
#     lambda { @s2.shutdown(3) }.should raise_error(ArgumentError)
#   end
end
