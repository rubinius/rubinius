require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/http_server"
require File.dirname(__FILE__) + "/shared/request_head"

describe "Net::HTTP#head2" do
  it_behaves_like :net_ftp_request_head, :head2
end

