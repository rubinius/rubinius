require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/size"

describe "Net::HTTPHeader#length" do
  it_behaves_like :net_httpheader_size, :length
end