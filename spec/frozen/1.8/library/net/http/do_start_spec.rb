require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/fixtures/http_server'

describe "Net::HTTP#do_start" do
  before(:all) do
    NetHTTPSpecs.start_server
  end
  
  after(:all) do
    NetHTTPSpecs.stop_server
  end

  before(:each) do
    @http = Net::HTTP.new("localhost", 3333)
  end

  it "is private" do
    Net::HTTP.private_instance_methods.should include("do_start")
  end

  it "opens the tcp connection to the current host" do
    @http.should_receive(:connect)
    @http.send(:do_start)
    @http.started?.should be_true
  end
end
