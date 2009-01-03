require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTPBadResponse" do
  it "is a subclass of StandardError" do
    Net::HTTPBadResponse.should < StandardError
  end
end
