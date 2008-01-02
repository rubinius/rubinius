require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#hour" do
  it "returns the hour of the day (0..23) for time" do
    with_timezone("CET", 1) do
      Time.at(0).hour.should == 1
    end
  end
end
