require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/shared/body"

describe "Net::HTTPResponse#entity" do
  it_behaves_like :net_httpresponse_body, :entity
end
