shared :time_to_i do |cmd|
  describe "Time##{cmd}" do
    it "returns the value of time as an integer number of seconds since epoch" do
      Time.at(0).send(cmd).should == 0
    end
  end
end
