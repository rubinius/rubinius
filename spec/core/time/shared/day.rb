shared :time_day do |cmd|
  describe "Time##{cmd}" do
    it "returns the day of the month (1..n) for time" do
      with_timezone("CET", 1) do
        Time.at(0).send(cmd).should == 1
      end
    end  
  end
end
