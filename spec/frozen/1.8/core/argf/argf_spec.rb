require File.dirname(__FILE__) + '/../../spec_helper'

describe "ARGF" do
  it "includes the Enumerable module" do
    Enumerable.should be_ancestor_of(ARGF.metaclass)
  end
end
