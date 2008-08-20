require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTP#on_connect" do
  before(:each) do
    @http = Net::HTTP.new("localhost", 3333)
  end
  
  it "is private" do
    Net::HTTP.private_instance_methods.should include("on_connect")
  end
  
  it "returns nil" do
    @http.send(:on_connect).should be_nil
  end
end
