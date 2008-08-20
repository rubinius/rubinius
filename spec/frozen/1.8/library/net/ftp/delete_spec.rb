require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#delete" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:readline).and_return("250 Requested file action okay, completed.")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "writes the DELE command to the socket" do
    @socket.should_receive(:write).with("DELE test.file\r\n")
    @ftp.delete("test.file")
  end
  
  it "does not raise any error when the response code is 250" do
    @socket.should_receive(:readline).and_return("250 Requested file action okay, completed.")
    @ftp.delete("test.file")
  end

  it "raises a Net::FTPTempError when the response code is 450" do
    @socket.should_receive(:readline).and_return("450 Requested file action not taken.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPTempError)
  end

  it "raises a Net::FTPPermError when the response code is 550" do
    @socket.should_receive(:readline).and_return("550 Requested action not taken.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPPermError)
  end
  
  it "raises a Net::FTPPermError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 502" do
    @socket.should_receive(:readline).and_return("502 Command not implemented.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPTempError)
  end

  it "raises a Net::FTPPermError when the response code is 530" do
    @socket.should_receive(:readline).and_return("530 Not logged in.")
    lambda { @ftp.delete("test.file") }.should raise_error(Net::FTPPermError)
  end
end