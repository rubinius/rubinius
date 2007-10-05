require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash" do
  it "includes Enumerable" do
    Hash.ancestors.include?(Enumerable).should == true
  end
end
