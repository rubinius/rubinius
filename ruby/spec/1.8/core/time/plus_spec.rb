require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#+" do
  it "increments the time by the specified amount" do
    (Time.at(0) + 100).should == Time.at(100)
  end
end
