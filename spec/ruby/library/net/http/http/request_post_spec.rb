require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/http_server"
require File.dirname(__FILE__) + "/shared/request_post"

describe "Net::HTTP#request_post" do
  it_behaves_like :net_ftp_request_post, :request_post
end