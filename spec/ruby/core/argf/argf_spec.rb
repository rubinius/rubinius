require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF" do
  it "is extended by the Enumerable module" do
    ARGF.should be_kind_of(Enumerable)
  end
end
