require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#to_proc" do
  it "returns self" do
    a = lambda {}
    a.to_proc.should equal(a)
  end
end