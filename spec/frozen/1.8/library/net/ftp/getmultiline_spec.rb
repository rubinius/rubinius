require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#getmultiline" do
  before(:each) do
    @socket = mock("Socket")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "is private" do
    @ftp.private_methods.should include("getmultiline")
  end
  
  it "returns single line responses read from the socket" do
    @socket.should_receive(:readline).and_return("200 Command okay.")
    @ftp.send(:getmultiline).should == "200 Command okay.\n"
  end

  it "returns multi line responses read from the socket" do
    responses = [
      "200-Start of multi line response.",
      "200-Multi line responses, yay!",
      "200 End of multi line response."
    ]
    
    @socket.should_receive(:readline).and_return(*responses)
    @ftp.send(:getmultiline).should == "200-Start of multi line response.\n200-Multi line responses, yay!\n200 End of multi line response.\n"
  end
  
  it "only reads till the end of a multi line response" do
    responses = [
      "200-Start of multi line response.",
      "200 End of multi line response.",
      "201 Ignored line."
    ]

    @socket.stub!(:readline).and_return(*responses)
    @ftp.send(:getmultiline).should == "200-Start of multi line response.\n200 End of multi line response.\n"
  end
  
  it "outputs the read lines after sanitizing when in debug mode" do
    @ftp.debug_mode = true
    
    responses = [
      "PASS password",
      "200-Start of multi line response.",
      "200 End of multi line response."
    ]
    
    @socket.should_receive(:readline).and_return(*responses)
    
    lambda { @ftp.send(:getmultiline) }.should output("get: PASS ********\n")
    lambda { @ftp.send(:getmultiline) }.should output("get: 200-Start of multi line response.\nget: 200 End of multi line response.\n")
  end
end
