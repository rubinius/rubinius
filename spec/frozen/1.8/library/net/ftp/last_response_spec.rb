require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#last_response" do
  before(:each) do
    @socket = mock("Socket")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "returns the last response" do
    responses = [ "200 Command okay.", "212 Directory status." ]
    @socket.should_receive(:readline).and_return(*responses)

    @ftp.send(:getresp)
    @ftp.last_response.should == "200 Command okay.\n"

    @ftp.send(:getresp)
    @ftp.last_response.should == "212 Directory status.\n"
  end
end
