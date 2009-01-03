require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/fixtures/http_server'

describe "Net::HTTP#lock" do
  before(:all) do
    NetHTTPSpecs.start_server
  end
  
  after(:all) do
    NetHTTPSpecs.stop_server
  end
  
  before(:each) do
    @http = Net::HTTP.start("localhost", 3333)
  end
  
  it "sends a LOCK request to the passed path and returns the response" do
    response = @http.lock("/request", "test=test")
    response.should be_kind_of(Net::HTTPResponse)
    response.body.should == "Request type: LOCK"
  end
end
