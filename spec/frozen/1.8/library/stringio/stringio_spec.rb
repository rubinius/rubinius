require File.dirname(__FILE__) + '/../../spec_helper'
require "stringio"

describe "StringIO" do
  it "should include the Enumerable module" do
    StringIO.should include(Enumerable)
  end
  
  it "is a subclass of Data" do
    StringIO.ancestors.should include(Data)
  end
end