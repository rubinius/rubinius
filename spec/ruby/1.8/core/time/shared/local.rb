shared :time_local do |cmd|
  describe "Time.#{cmd}" do
    it "local should create a time based on given values, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end

    it "local should create a time based on given C-style gmtime arguments, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end

    it "should be able to handle string-like second argument" do
      with_timezone("PST", -8) do
        Time.send(cmd, 2008, "12").inspect.should == "Mon Dec 01 00:00:00 -0800 2008"
        Time.send(cmd, 2008, "apr").inspect.should == "Tue Apr 01 00:00:00 -0800 2008"
        (obj = mock('12')).should_receive(:to_str).and_return("12")
        Time.send(cmd, 2008, obj).inspect.should == "Mon Dec 01 00:00:00 -0800 2008"
        fails_on(:ruby) do
          # Exclude MRI 1.8.6 because it sigfaults. :)
          # But the problem is fixed in MRI repository already.
          (obj = mock('apr')).should_receive(:to_str).and_return('apr')
          Time.send(cmd, 2008, obj).inspect.should == "Tue Apr 01 00:00:00 -0800 2008"
        end
      end
    end
  end
end
