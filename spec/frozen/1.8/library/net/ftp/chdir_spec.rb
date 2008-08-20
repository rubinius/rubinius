require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#chdir when switching to the parent directory" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:readline).and_return("200 Command okay.")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "writes the 'CDUP' command to the socket" do
    @socket.should_receive(:write).with("CDUP\r\n")
    @ftp.chdir("..")
  end

  it "does not raise any error when the response code is 200" do
    @socket.should_receive(:readline).and_return("200 Command okay.")
    @ftp.chdir("..")
  end

  # BUG: These raise a NoMethodError
  # 
  # it "raises a Net::FTPProtoError when the response code is 500" do
  #   @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 501" do
  #   @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 502" do
  #   @socket.should_receive(:readline).and_return("502 Command not implemented.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 421" do
  #   @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 530" do
  #   @socket.should_receive(:readline).and_return("530 Not logged in.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 550" do
  #   @socket.should_receive(:readline).and_return("550 Requested action not taken.")
  #   lambda { @ftp.chdir("..") }.should raise_error(Net::FTPProtoError)
  # end
end

describe "Net::FTP#chdir" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:readline).and_return("200 Command okay.")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "writes the 'CWD' command with the passed directory to the socket" do
    @socket.should_receive(:write).with("CWD test\r\n")
    @ftp.chdir("test")
  end

  it "does not raise any error when the response code is 200" do
    @socket.should_receive(:readline).and_return("200 Command okay.")
    @ftp.chdir("test")
  end

  # BUG: These raise a NoMethodError
  #
  # it "raises a Net::FTPProtoError when the response code is 500" do
  #   @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 501" do
  #   @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 502" do
  #   @socket.should_receive(:readline).and_return("502 Command not implemented.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 421" do
  #   @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 530" do
  #   @socket.should_receive(:readline).and_return("530 Not logged in.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
  # 
  # it "raises a Net::FTPProtoError when the response code is 550" do
  #   @socket.should_receive(:readline).and_return("550 Requested action not taken.")
  #   lambda { @ftp.chdir("test") }.should raise_error(Net::FTPProtoError)
  # end
end
