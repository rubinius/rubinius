require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#min" do
  it "returns the minute of the hour (0..59) for time" do
    with_timezone("CET", 1) do
      Time.at(0).min.should == 0
    end
  end
end
