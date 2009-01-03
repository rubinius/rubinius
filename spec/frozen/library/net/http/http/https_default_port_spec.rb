require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'

describe "Net::HTTP.https_default_port" do
  it "returns 443" do
    Net::HTTP.https_default_port.should eql(443)
  end
end
