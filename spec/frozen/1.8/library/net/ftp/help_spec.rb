require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#help" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:readline).and_return("226 Success")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "writes the HELP command to the socket" do
    @socket.should_receive(:write).with("HELP\r\n")
    @ftp.help
  end

  it "writes the HELP command with an optional parameter to the socket" do
    @socket.should_receive(:write).with("HELP some parameter\r\n")
    @ftp.help("some parameter")
  end

  it "does not raise any error when the response code is 211" do
    @socket.should_receive(:readline).and_return("211 System status, or system help reply.")
    @ftp.help
  end

  it "does not raise any error when the response code is 214" do
    @socket.should_receive(:readline).and_return("214 Help message.")
    @ftp.help
  end

  it "raises a Net::FTPPermError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.help }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.help }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 502" do
    @socket.should_receive(:readline).and_return("502 Command not implemented.")
    lambda { @ftp.help }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.help }.should raise_error(Net::FTPTempError)
  end
end
