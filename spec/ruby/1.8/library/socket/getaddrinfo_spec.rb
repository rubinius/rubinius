require File.dirname(__FILE__) + '/../../spec_helper'

describe "Socket#getaddrinfo" do

  it "gets the address information" do
    expected = []

    if Socket.constants.include? 'AF_INET6' then
      expected.concat [
        ['AF_INET6', 80, 'localhost', '::1', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, 'localhost', '::1', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
        ['AF_INET6', 80, 'localhost', '::1', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, 'localhost', '::1', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
      ]
    end

    expected.concat [
      ['AF_INET', 80, 'localhost', '127.0.0.1', Socket::AF_INET,
        Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
      ['AF_INET', 80, 'localhost', '127.0.0.1', Socket::AF_INET,
        Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
    ]

    addrinfo = Socket.getaddrinfo 'localhost', 'http'

    addrinfo.should == expected
  end

end

