require File.dirname(__FILE__) + '/../../spec_helper'
require 'uri'

describe "URI#==" do
  it "ignores capitalization of host names and scheme names" do
    URI("http://exAMPLE.cOm").should == URI("http://example.com")
  end
  
  it "treats a blank path and a path of '/' as the same" do
    URI("http://example.com").should == URI("http://example.com/")
  end
  
  it "is case sensitive in all components of the URI but the host and scheme" do
    URI("hTTp://example.com").should_not == URI("http://example.com")    
    URI("http://example.com/paTH").should_not == URI("http://example.com/path")
    URI("http://uSer@example.com").should_not == URI("http://user@example.com")            
    URI("http://example.com/path?quERy").should_not == URI("http://example.com/?query")
    URI("http://example.com/#fragMENT").should_not == URI("http://example.com#fragment")            
  end
  
  it "differentiates based on port number" do
    URI("http://example.com:8080").should_not == URI("http://example.com")
  end
end