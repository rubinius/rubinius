require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#hour" do
  it "returns the hour of the day (0..23) for time" do
    with_timezone("CET", 1) do
      Time.at(0).hour.should == 1
    end
  end
end
