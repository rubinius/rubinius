require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"
require File.dirname(__FILE__) + "/shared/puttextfile"
require File.dirname(__FILE__) + "/shared/putbinaryfile"

describe "Net::FTP#put (binary mode)" do
  before(:each) do
    @binary_mode = true
  end
  
  it_behaves_like :net_ftp_putbinaryfile, :put
end

describe "Net::FTP#put (text mode)" do
  before(:each) do
    @binary_mode = false
  end
  
  it_behaves_like :net_ftp_puttextfile, :put
end
