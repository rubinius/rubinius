require File.dirname(__FILE__) + '/../../spec_helper'

describe "Dir" do
  it "includes Enumerable" do
    Dir.include?(Enumerable).should == true
  end
end
