require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct#initialize" do
  it "is private" do
    OpenStruct.private_instance_methods.should include("initialize")
  end
end
