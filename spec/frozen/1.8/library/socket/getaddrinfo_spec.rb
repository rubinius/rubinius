require File.dirname(__FILE__) + '/../../spec_helper'
require 'socket'

describe "Socket#getaddrinfo" do
  it "gets the address information" do
    expected = []
    host = Socket.getaddrinfo("127.0.0.1", nil)[0][2]
    # The check for AP_INET6's class is needed because ipaddr.rb adds
    # fake AP_INET6 even in case when IPv6 is not really supported.
    # Without such check, this test might fail when ipaddr was required
    # by some other specs.
    if (Socket.constants.include? 'AF_INET6') &&
        (Socket::AF_INET6.class != Object) then
      expected.concat [
        ['AF_INET6', 80, host, '::1', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, host, '::1', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
        ['AF_INET6', 80, host, '::1', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, host, '::1', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
        ['AF_INET6', 80, host, 'fe80::1%lo0', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, host, 'fe80::1%lo0', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
        ['AF_INET6', 80, host, 'fe80::1%lo0', Socket::AF_INET6,
          Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
        ['AF_INET6', 80, host, 'fe80::1%lo0', Socket::AF_INET6,
          Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
      ]
    end

    expected.concat [
      ['AF_INET', 80, host, '127.0.0.1', Socket::AF_INET,
        Socket::SOCK_DGRAM, Socket::IPPROTO_UDP],
      ['AF_INET', 80, host, '127.0.0.1', Socket::AF_INET,
        Socket::SOCK_STREAM, Socket::IPPROTO_TCP],
    ]

    addrinfo = Socket.getaddrinfo host, 'http'
    addrinfo.each { |a| expected.should include(a) }
  end

end

