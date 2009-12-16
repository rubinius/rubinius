require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/http_server"
require File.dirname(__FILE__) + "/shared/request_put"

describe "Net::HTTP#request_put" do
  it_behaves_like :net_ftp_request_put, :request_put
end
