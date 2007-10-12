@time_isdst = shared "Time#isdst" do |cmd|
  describe "Time##{cmd}" do
    it "dst? returns whether time is during daylight saving time" do
      Time.local(2007, 9, 9, 0, 0, 0).send(cmd).should == true
      Time.local(2007, 1, 9, 0, 0, 0).send(cmd).should == false
    end
  end
end
