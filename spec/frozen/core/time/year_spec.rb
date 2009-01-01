require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#year" do
  it "returns the four digit year for time as an integer" do
    with_timezone("CET", 1) do
      Time.at(0).year.should == 1970
    end
  end  
end
