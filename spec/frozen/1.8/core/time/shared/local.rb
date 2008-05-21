shared :time_local do |cmd|
  describe "Time.#{cmd}" do
    it "creates a time based on given values, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end

    it "creates a time based on given C-style gmtime arguments, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end
  end
end
