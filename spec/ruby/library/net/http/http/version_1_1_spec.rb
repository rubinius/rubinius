require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/shared/version_1_1'

describe "Net::HTTP.version_1_1" do
  it "turns on net/http 1.1 features" do
    Net::HTTP.version_1_1
    
    Net::HTTP.version_1_1?.should be_true
    Net::HTTP.version_1_2?.should be_false
  end
  
  it "returns false" do
    Net::HTTP.version_1_1.should be_false
  end
end

describe "Net::HTTP.version_1_1?" do
  it_behaves_like :net_http_version_1_1_p, :version_1_1?
end
