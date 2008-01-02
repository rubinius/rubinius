require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#zone" do
  it "returns the time zone used for time" do
    # Testing with Asia/Kuwait here because it doesn't have DST.
    with_timezone("Asia/Kuwait") do
      Time.now.zone.should == "AST"
    end
  end
end
