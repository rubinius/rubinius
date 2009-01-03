require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"

describe "Net::FTP#quit" do
  before(:each) do
    @server = NetFTPSpecs::DummyFTP.new
    @server.serve_once

    @ftp = Net::FTP.new
    @ftp.connect("localhost", 9921)
  end

  after(:each) do
    @ftp.quit rescue nil
    @ftp.close
    @server.stop
  end

  it "sends the QUIT command to the server" do
    @ftp.quit
    @ftp.last_response.should == "221 OK, bye\n"
  end
  
  it "does not close the socket automagically" do
    @ftp.quit
    @ftp.closed?.should be_false
  end
  
  it "returns nil" do
    @ftp.quit.should be_nil
  end
end
