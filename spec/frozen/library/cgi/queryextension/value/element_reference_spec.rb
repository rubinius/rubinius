require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'cgi'

describe "CGI::QueryExtension::Value#[] when passed index" do
  before(:each) do
    @value = "some value"
    @value.extend(CGI::QueryExtension::Value)
    @value.set_params(["some value", "other value", "third value"])
  end

  it "returns the alternative value with the passed index" do
    @value[0].should == "some value"
    @value[1].should == "other value"
    @value[2].should == "third value"
  end
end
