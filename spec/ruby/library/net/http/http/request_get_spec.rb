require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/http_server"
require File.dirname(__FILE__) + "/shared/request_get"

describe "Net::HTTP#request_get" do
  it_behaves_like :net_ftp_request_get, :get2
end
