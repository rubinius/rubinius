shared :matchdata_length do |cmd|
  describe "MatchData##{cmd}" do
    it "length should return the number of elements in the match array" do
      /(.)(.)(\d+)(\d)/.match("THX1138.").send(cmd).should == 5
    end
  end
end
