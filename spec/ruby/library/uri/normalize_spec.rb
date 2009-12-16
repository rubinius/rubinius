require File.dirname(__FILE__) + '/../../spec_helper'
require 'uri'

describe "URI#normalize" do
  it "adds a / onto the end of the URI if the path is blank" do
    no_path = URI("http://example.com")
    no_path.to_s.should_not == "http://example.com/"
    no_path.normalize.to_s.should == "http://example.com/"
  end
  
  it "downcases the host of the URI" do
    uri = URI("http://exAMPLE.cOm/")
    uri.to_s.should_not == "http://example.com/"
    uri.normalize.to_s.should == "http://example.com/"
  end
end