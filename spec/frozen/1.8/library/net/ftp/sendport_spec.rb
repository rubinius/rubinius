require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#sendport when using IPv4" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:peeraddr).and_return(["AF_INET"])
    @socket.stub!(:readline).and_return("200 Command okay.")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "sends a HOST-PORT specification for the passed host and port using the PORT command" do
    @socket.should_receive(:write).with("PORT 192,168,0,1,4,210\r\n")
    @ftp.send(:sendport, "192.168.0.1", 1234)
  end
  
  it "raises a Net::FTPPermError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPTempError)
  end

  it "raises a Net::FTPPermError when the response code is 530" do
    @socket.should_receive(:readline).and_return("530 Not logged in.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end
end

describe "Net::FTP#sendport when using IPv6" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:peeraddr).and_return(["AF_INET6"])
    @socket.stub!(:readline).and_return("200 Command okay.")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "sends a HOST-PORT specification for the passed host and port using the EPRT command" do
    @socket.should_receive(:write).with("EPRT |2|192.168.0.1|1234|\r\n")
    @ftp.send(:sendport, "192.168.0.1", 1234)
  end

  it "raises a Net::FTPPermError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPTempError)
  end

  it "raises a Net::FTPPermError when the response code is 530" do
    @socket.should_receive(:readline).and_return("530 Not logged in.")
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPPermError)
  end
end

describe "Net::FTP#sendport when the Address family is neither AF_INET nor AF_INET6" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:peeraddr).and_return(["AF_UNSPEC"])

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "raises a Net::FTPProtoError" do
    lambda { @ftp.send(:sendport, "192.168.0.1", 1234) }.should raise_error(Net::FTPProtoError)
  end
end