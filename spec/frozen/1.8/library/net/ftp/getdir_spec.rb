require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/shared/pwd"

describe "Net::FTP#getdir" do
  it_behaves_like :net_ftp_pwd, :getdir
end
