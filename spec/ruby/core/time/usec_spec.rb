require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#usec" do
  it "returns the microseconds for time" do
    Time.at(0).usec.should == 0
    (Time.at(1.1) + 0.9).usec.should == 0
    (Time.at(1.1) - 0.2).usec.should == 900000
  end
end
