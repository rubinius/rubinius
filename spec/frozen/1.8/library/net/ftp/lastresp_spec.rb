require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/shared/last_response_code"
require File.dirname(__FILE__) + "/fixtures/server"

describe "Net::FTP#lastresp" do
  it_behaves_like :net_ftp_last_response_code, :lastresp
end
