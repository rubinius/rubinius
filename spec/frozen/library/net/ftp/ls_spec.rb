require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"
require File.dirname(__FILE__) + '/shared/list'

describe "Net::FTP#ls" do
  it_behaves_like :net_ftp_list, :ls
end
