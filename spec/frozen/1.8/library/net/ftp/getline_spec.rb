require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#getline" do
  before(:each) do
    @socket = mock("Socket")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "is private" do
    @ftp.private_methods.should include("getline")
  end
  
  it "reads one line from the socket" do
    @socket.should_receive(:readline).and_return("200 Command okay.")
    @ftp.send(:getline)
  end
  
  it "returns the read line with removed line feeds" do
    responses = [
      "200 Command okay.\n", "200 Command okay.\n\n\n",
      "200 Command okay.\r\n", "200 Command okay.\r\n\r\n"
    ]
    @socket.should_receive(:readline).and_return(*responses)
    
    @ftp.send(:getline).should == "200 Command okay."
    @ftp.send(:getline).should == "200 Command okay.\n\n"
    @ftp.send(:getline).should == "200 Command okay."
    @ftp.send(:getline).should == "200 Command okay.\r\n"
  end
  
  it "outputs the sanitized line when in debug mode" do
    @ftp.debug_mode = true
    
    @socket.should_receive(:readline).and_return("PASS password")
    lambda { @ftp.send(:getline) }.should output("get: PASS ********\n")
  end
end
