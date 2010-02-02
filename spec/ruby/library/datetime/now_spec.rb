require File.dirname(__FILE__) + '/../../spec_helper'
require 'date'

describe "DateTime.now" do
  it "creates an instance of DateTime" do
    DateTime.now.should be_an_instance_of(DateTime)
  end
end
