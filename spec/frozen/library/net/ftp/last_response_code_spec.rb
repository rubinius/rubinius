require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/shared/last_response_code"
require File.dirname(__FILE__) + "/fixtures/server"

describe "Net::FTP#last_response_code" do
  it_behaves_like :net_ftp_last_response_code, :last_response_code
end
