require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe "Socket#new" do
  it "only allows Socket constants as strings" do
    [ :DGRAM, :RAW, :RDM, :SEQPACKET, :STREAM ].each do |socket_type|
      lambda { Socket.new(Socket::PF_UNIX, "SOCK_#{socket_type}", 0) }.should_not raise_error(SocketError)
    end
  end

  ruby_version_is ""..."1.9" do
    it "raises Errno::EPROTONOSUPPORT if socket type is not a string or integer" do
      lambda { Socket.new(Socket::PF_UNIX, :DGRAM, 0) }.should raise_error(Errno::EPROTONOSUPPORT)
    end
  end

  ruby_version_is "1.9" do
    it "raises SocketError if given symbol is not a Socket constants reference" do
      lambda { Socket.new(Socket::PF_UNIX, :NO_EXIST, 0) }.should raise_error(SocketError)
    end

    it "only allows Socket constants as symbols" do
      [ :DGRAM, :RAW, :RDM, :SEQPACKET, :STREAM ].each do |socket_type|
        lambda { Socket.new(Socket::PF_UNIX, socket_type, 0) }.should_not raise_error(SocketError)
      end
    end
  end
end
