require File.dirname(__FILE__) + '/../../spec_helper'

require 'resolv'
require 'socket'

describe "Resolv#getaddress" do
  it 'resolves localhost' do

    localhost = Socket.getaddrinfo("127.0.0.1", nil)[0][2]

    lambda {
      address = Resolv.getaddress(localhost)
    }.should_not raise_error(Resolv::ResolvError)

    lambda {
      address = Resolv.getaddress("should.raise.error")
    }.should raise_error(Resolv::ResolvError)
  end

end
