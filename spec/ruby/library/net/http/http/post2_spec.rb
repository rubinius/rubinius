require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/shared/request_post"

describe "Net::HTTP#post2" do
  it_behaves_like :net_ftp_request_post, :post2
end