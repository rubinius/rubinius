require File.expand_path('../../../spec_helper', __FILE__)

describe "MatchData#begin" do
  it "returns the offset of the start of the nth element" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(0).should == 1
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(2).should == 2
  end
end
