require File.dirname(__FILE__) + '/../../spec_helper'

@matchdata_length = shared "MatchData#length" do |cmd|
  describe "MatchData##{cmd}" do
    it "length should return the number of elements in the match array" do
      /(.)(.)(\d+)(\d)/.match("THX1138.").send(cmd).should == 5
    end
  end
end

describe "MatchData#length" do
  it_behaves_like(@matchdata_length, :length)
end
