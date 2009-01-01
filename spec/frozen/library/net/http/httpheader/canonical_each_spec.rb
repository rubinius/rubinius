require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + "/fixtures/classes"
require File.dirname(__FILE__) + "/shared/each_capitalized"

describe "Net::HTTPHeader#canonical_each" do
  it_behaves_like :net_httpheader_each_capitalized, :canonical_each
end
