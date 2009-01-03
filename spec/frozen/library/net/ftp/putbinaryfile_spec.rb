require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"
require File.dirname(__FILE__) + "/shared/putbinaryfile"

describe "Net::FTP#putbinaryfile" do
  it_behaves_like :net_ftp_putbinaryfile, :putbinaryfile
end
