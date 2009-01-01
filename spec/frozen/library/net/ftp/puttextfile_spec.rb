require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"
require File.dirname(__FILE__) + "/shared/puttextfile"

describe "Net::FTP#puttextfile" do
  it_behaves_like :net_ftp_puttextfile, :puttextfile
end