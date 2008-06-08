require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#getresp" do
  before(:each) do
    @socket = mock("Socket")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "returns single line responses when the response code begins with 1" do
    @socket.should_receive(:readline).and_return("1xx Does not raise.")
    @ftp.send(:getresp).should == "1xx Does not raise.\n"
  end

  it "returns multi line responses when the response code begins with 1" do
    @socket.should_receive(:readline).and_return("1xx-Does not raise.", "1xx Does not raise.")
    @ftp.send(:getresp).should == "1xx-Does not raise.\n1xx Does not raise.\n"
  end

  it "returns single line response when the response code begins with 2" do
    @socket.should_receive(:readline).and_return("2xx Does not raise.")
    @ftp.send(:getresp).should == "2xx Does not raise.\n"
  end

  it "returns multi line responses when the response code begins with 2" do
    @socket.should_receive(:readline).and_return("2xx-Does not raise.", "2xx Does not raise.")
    @ftp.send(:getresp).should == "2xx-Does not raise.\n2xx Does not raise.\n"
  end

  it "returns single line response when the response code begins with 3" do
    @socket.should_receive(:readline).and_return("3xx Does not raise.")
    @ftp.send(:getresp).should == "3xx Does not raise.\n"
  end

  it "returns multi line responses when the response code begins with 3" do
    @socket.should_receive(:readline).and_return("3xx-Does not raise.", "3xx Does not raise.")
    @ftp.send(:getresp).should == "3xx-Does not raise.\n3xx Does not raise.\n"
  end

  it "raises a Net::FTPTempError when the response code begins with 4" do
    @socket.should_receive(:readline).and_return("4xx A temporary error occured.")
    lambda { @ftp.send(:getresp) }.should raise_error(Net::FTPTempError)
  end
  
  it "raises a Net::FTPPermError when the response code begins with 5" do
    @socket.should_receive(:readline).and_return("5xx A permission error occured.")
    lambda { @ftp.send(:getresp) }.should raise_error(Net::FTPPermError)
  end
end
