shared :time_gm do |cmd|
  describe "Time.#{cmd}" do
    it "creates a time based on given values, interpreted as UTC (GMT)" do
      Time.send(cmd, 2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
    end

    it "creates a time based on given C-style gmtime arguments, interpreted as UTC (GMT)" do
      Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
    end
  end
end
