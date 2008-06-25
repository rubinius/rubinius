require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPSocket#gethostbyname" do

  before :each do
    @host_info = TCPSocket.gethostbyname("localhost")
  end
  it "returns an array with 4 elements of information on the hostname" do
    @host_info.should be_kind_of(Array)
    @host_info.length.should == 4
  end

  it "returns the canonical name as first value" do
    @host_info[0].should == "localhost"
  end

  it "returns any aliases to the address as second value" do
    @host_info[1].should be_kind_of(Array)
  end

  not_compliant_on :jruby do
    it "returns the address type as the third value" do
      address_type = @host_info[2]
      [Socket::AF_INET, Socket::AF_INET6].include?(address_type).should be_true
    end

    it "returns the IP address as the fourth value" do
      ip = @host_info[3]
      ["127.0.0.1", "::1"].include?(ip).should be_true
    end
  end


  compliant_on :jruby do
    it "returns the address type as the third value" do
      @host_info[2].should == Socket::AF_INET
    end

    it "returns the IP address as the fourth value" do
      @host_info[3].should == "127.0.0.1"
    end
  end

end
