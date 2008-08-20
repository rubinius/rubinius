require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/fixtures/http_server'

describe "Net::HTTP#head" do
  before(:all) do
    NetHTTPSpecs.start_server
  end
  
  after(:all) do
    NetHTTPSpecs.stop_server
  end
  
  before(:each) do
    @http = Net::HTTP.start("localhost", 3333)
  end
  
  it "sends a HEAD request to the passed path and returns the response" do
    response = @http.head("/request")
    response.should be_kind_of(Net::HTTPResponse)
    # HEAD requests have no responses
    response.body.should be_nil
  end
end
