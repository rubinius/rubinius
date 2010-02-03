require File.dirname(__FILE__) + '/../../spec_helper'
require 'socket'

describe "Resolv#getaddresses" do
  before(:all) do
    require 'resolv'
  end

  it 'resolves localhost' do
    res = Resolv.new([Resolv::Hosts.new])

    addresses = nil

    lambda {
      addresses = res.getaddresses("localhost")
    }.should_not raise_error(Resolv::ResolvError)

    addresses.should_not == nil
    addresses.size.should > 0
  end

end
