require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/set_range"

describe "Net::HTTPHeader#set_range" do
  it_behaves_like :net_httpheader_set_range, :set_range
end
