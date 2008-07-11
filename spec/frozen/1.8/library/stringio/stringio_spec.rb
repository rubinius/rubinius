require File.dirname(__FILE__) + '/../../spec_helper'
require "stringio"

describe "StringIO" do
  it "should include the Enumerable module" do
    StringIO.should include(Enumerable)
  end
end

