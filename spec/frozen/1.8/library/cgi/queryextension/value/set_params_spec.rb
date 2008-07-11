require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'cgi'

describe "CGI::QueryExtension::Value#set_params" do
  before(:each) do
    @value = "some value"
    @value.extend(CGI::QueryExtension::Value)
  end

  it "sets alternative params" do
    @value.set_params(["some value", "other value", "third value"])
    @value.to_a.should == ["some value", "other value", "third value"]
  end
end
