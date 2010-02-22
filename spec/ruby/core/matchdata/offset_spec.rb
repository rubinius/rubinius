require File.expand_path('../../../spec_helper', __FILE__)

describe "MatchData#offset" do
  it "returns a two element array with the begin and end of the nth match" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").offset(0).should == [1, 7]
    /(.)(.)(\d+)(\d)/.match("THX1138.").offset(4).should == [6, 7]
  end
end
