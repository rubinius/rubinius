require File.dirname(__FILE__) + '/../../spec_helper'

describe "Object.new" do
  specify "creates a new Object" do
    Object.new.class.should == Object
  end
end

