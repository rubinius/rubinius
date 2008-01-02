shared :time_gmt_offset do |cmd|
  describe "Time##{cmd}" do
    it "returns the offset in seconds between the timezone of time and UTC" do
      with_timezone("AST", 3) do
        Time.new.send(cmd).should == 10800
      end
    end
  end
end
