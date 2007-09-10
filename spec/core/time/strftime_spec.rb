require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#strftime" do
  it "formats time according to the directives in the given format string" do
    Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
  end
end
