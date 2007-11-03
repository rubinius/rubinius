shared :time_month do |cmd|
  describe "Time##{cmd}" do
    it "returns the month of the year" do
      Time.at(99999).mon.should == 1
    end
  end
end
