require File.dirname(__FILE__) + '/../../spec_helper'
require 'socket'

describe "Resolv#getaddresses" do
  before(:all) do
    require 'resolv'
  end

  it 'resolves localhost' do
    localhost = Socket.getaddrinfo("127.0.0.1", nil)[0][2]

    addresses = nil

    lambda {
      addresses = Resolv.getaddresses(localhost)
    }.should_not raise_error(Resolv::ResolvError)
    
    addresses.should_not == nil
    addresses.size.should > 0
  end

end
