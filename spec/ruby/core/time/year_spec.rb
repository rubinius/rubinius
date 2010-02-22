require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#year" do
  it "returns the four digit year for time as an integer" do
    with_timezone("CET", 1) do
      Time.at(0).year.should == 1970
    end
  end  
end
