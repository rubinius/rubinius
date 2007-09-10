require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#hash" do
  it "returns a unique integer for each time" do
    Time.at(100).hash.should == 100
    Time.at(100, 123456).hash.should == 123428
    Time.gm(1980).hash.should == 315532800
  end
end
