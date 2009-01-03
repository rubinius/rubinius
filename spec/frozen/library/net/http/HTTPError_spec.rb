require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTPError" do
  it "is a subclass of Net::ProtocolError" do
    Net::HTTPError.should < Net::ProtocolError
  end
  
  it "includes the Net::HTTPExceptions module" do
    Net::HTTPError.should < Net::HTTPExceptions
  end
end
