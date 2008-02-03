shared :time_params do |cmd|
  describe "Time.#{cmd}" do
    it "handles string-like second argument" do
      Time.send(cmd, 2008, "12").should  == Time.send(cmd, 2008, 12)
      Time.send(cmd, 2008, "dec").should == Time.send(cmd, 2008, 12)
      (obj = mock('12')).should_receive(:to_str).and_return("12")
      Time.send(cmd, 2008, obj).should == Time.send(cmd, 2008, 12)
      fails_on(:ruby) do
        # Exclude MRI 1.8.6 because it segfaults. :)
        # But the problem is fixed in MRI repository already.
        (obj = mock('dec')).should_receive(:to_str).and_return('dec')
        Time.send(cmd, 2008, obj).should == Time.send(cmd, 2008, 12)
      end
    end

    it "handles string arguments" do
      Time.send(cmd, "2000", "1", "1" , "20", "15", "1").should == Time.send(cmd, 2000, 1, 1, 20, 15, 1)
      Time.send(cmd, "1", "15", "20", "1", "1", "2000", :ignored, :ignored, :ignored, :ignored).should == Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored)
    end

    it "handles float arguments" do
      Time.send(cmd, 2000.0, 1.0, 1.0, 20.0, 15.0, 1.0).should == Time.send(cmd, 2000, 1, 1, 20, 15, 1)
      Time.send(cmd, 1.0, 15.0, 20.0, 1.0, 1.0, 2000.0, :ignored, :ignored, :ignored, :ignored).should == Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored)
    end

    it "should accept various year ranges" do
      lambda { Time.send(cmd, 1901, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
      lambda { Time.send(cmd, 2037, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
      
      lambda { Time.send(cmd, 1901, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
      lambda { Time.send(cmd, 1901, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
      
      
      if defined? Rubinius && Rubinius::WORDSIZE == 32
        lambda { Time.send(cmd, 1900, 12, 31, 23, 59, 59, 0) }.should raise_error(ArgumentError) # mon
        lambda { Time.send(cmd, 2038, 12, 31, 23, 59, 59, 0) }.should raise_error(ArgumentError) # mon
      end
      
      if defined? Rubinius && Rubinius::WORDSIZE == 64
        lambda { Time.send(cmd, 1900, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
        lambda { Time.send(cmd, 2038, 12, 31, 23, 59, 59, 0) }.should_not raise_error(ArgumentError) # mon
      end
      
    end

    it "throws ArgumentError for out of range values" do
      # year-based Time.local(year (, month, day, hour, min, sec, usec))
      # Year range only fails on 32 bit archs
      if defined? Rubinius && Rubinius::WORDSIZE == 32
        lambda { Time.send(cmd, 1111, 12, 31, 23, 59, 59, 0) }.should raise_error(ArgumentError) # year
      end
      lambda { Time.send(cmd, 2008, 13, 31, 23, 59, 59, 0) }.should raise_error(ArgumentError) # mon
      lambda { Time.send(cmd, 2008, 12, 32, 23, 59, 59, 0) }.should raise_error(ArgumentError) # day
      lambda { Time.send(cmd, 2008, 12, 31, 25, 59, 59, 0) }.should raise_error(ArgumentError) # hour
      lambda { Time.send(cmd, 2008, 12, 31, 23, 61, 59, 0) }.should raise_error(ArgumentError) # min
      lambda { Time.send(cmd, 2008, 12, 31, 23, 59, 61, 0) }.should raise_error(ArgumentError) # sec

      # second based Time.local(sec, min, hour, day, month, year, wday, yday, isdst, tz)
      lambda { Time.send(cmd, 61, 59, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # sec
      lambda { Time.send(cmd, 59, 61, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # min
      lambda { Time.send(cmd, 59, 59, 25, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # hour
      lambda { Time.send(cmd, 59, 59, 23, 32, 12, 2008, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # day
      lambda { Time.send(cmd, 59, 59, 23, 31, 13, 2008, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # month
      # Year range only fails on 32 bit archs
      if defined? Rubinius && Rubinius::WORDSIZE == 32
        lambda { Time.send(cmd, 59, 59, 23, 31, 12, 1111, :ignored, :ignored, :ignored, :ignored) }.should raise_error(ArgumentError) # year
      end
    end

    it "throws ArgumentError for invalid number of arguments" do
      # Time.local only takes either 1-8, or 10 arguments
      lambda { Time.send(cmd, 59, 1, 2, 3, 4, 2008, 0, 0, 0) }.should raise_error(ArgumentError) # 9 go boom

      lambda { Time.send(cmd, 2008)                                                         }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12)                                                     }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12, 31)                                                 }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12, 31, 23)                                             }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12, 31, 23, 59)                                         }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12, 31, 23, 59, 59)                                     }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 2008, 12, 31, 23, 59, 59, 0)                                  }.should_not raise_error(ArgumentError)
      lambda { Time.send(cmd, 59, 1, 2, 3, 4, 2008, :ignored, :ignored, :ignored, :ignored) }.should_not raise_error(ArgumentError)
    end
  end
end
