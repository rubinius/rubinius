@time_asctime = shared "Time#asctime" do |cmd|
  describe "Time##{cmd}" do
    it "returns a canonical string representation of time" do
      t = Time.now
      t.send(cmd).should == t.strftime("%a %b %e %H:%M:%S %Y")
    end
  end
end
