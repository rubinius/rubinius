require File.dirname(__FILE__) + '/../../spec_helper'

describe "MatchData#begin" do
  it "returns the offset of the start of the nth element" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(0).should == 1
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(2).should == 2
  end
end
