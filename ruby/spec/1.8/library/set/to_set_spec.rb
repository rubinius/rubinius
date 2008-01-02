require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Enumerable#to_set" do
  it "is added by Set" do
    Enumerable.instance_methods.should include("to_set")
  end
end
