require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/each_header"

describe "Net::HTTPHeader#each_header" do
  it_behaves_like :net_httpheader_each_header, :each_header
end
