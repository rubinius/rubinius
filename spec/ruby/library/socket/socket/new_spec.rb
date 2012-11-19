require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe "Socket#new" do
  ruby_version_is "1.9" do
    it "raises SocketError if given symbol is not a Socket constants reference" do
      lambda { Socket.new(Socket::PF_UNIX, :NO_EXIST, 0) }.should raise_error(SocketError)
    end

    it "only allows Socket constants as symbols" do
      [ :DGRAM, :RAW, :RDM, :SEQPACKET, :STREAM ].each do |socket_type|
        lambda { Socket.new(Socket::PF_UNIX, socket_type, 0) }.should_not raise_error(SocketError)
      end
    end

    it "only allows Socket constants as strings" do
      [ :DGRAM, :RAW, :RDM, :SEQPACKET, :STREAM ].each do |socket_type|
        lambda { Socket.new(Socket::PF_UNIX, "SOCK_#{socket_type}", 0) }.should_not raise_error(SocketError)
      end
    end
  end
end
