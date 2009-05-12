require File.dirname(__FILE__) + '/../../spec_helper'
require "stringio"

describe "StringIO" do
  it "includes the Enumerable module" do
    StringIO.should include(Enumerable)
  end
end

