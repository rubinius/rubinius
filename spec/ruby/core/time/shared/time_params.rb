describe :time_params, :shared => true do
  it "handles string-like second argument" do
    Time.send(@method, 2008, "12").should  == Time.send(@method, 2008, 12)
    Time.send(@method, 2008, "dec").should == Time.send(@method, 2008, 12)
    (obj = mock('12')).should_receive(:to_str).and_return("12")
    Time.send(@method, 2008, obj).should == Time.send(@method, 2008, 12)
  end

  ruby_bug "#", "1.8.6.114" do
    # Exclude MRI 1.8.6 because it segfaults. :)
    # But the problem is fixed in MRI repository already.
    it "handles string-like second argument" do
      (obj = mock('dec')).should_receive(:to_str).and_return('dec')
      Time.send(@method, 2008, obj).should == Time.send(@method, 2008, 12)
    end
  end

  it "handles string arguments" do
    Time.send(@method, "2000", "1", "1" , "20", "15", "1").should ==
      Time.send(@method, 2000, 1, 1, 20, 15, 1)
    # "08" "09" are special; make sure they are *not* treated as octal when strings
    Time.send(@method, "2000", "08", "08" , "08", "08", "08").should ==
      Time.send(@method, 2000, 8, 8, 8, 8, 8)
    Time.send(@method, "2000", "09", "09" , "09", "09", "09").should ==
      Time.send(@method, 2000, 9, 9, 9, 9, 9)
    Time.send(@method, "1", "15", "20", "1", "1", "2000", :ignored, :ignored,
              :ignored, :ignored).should ==
      Time.send(@method, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored)
  end

  it "handles float arguments" do
    Time.send(@method, 2000.0, 1.0, 1.0, 20.0, 15.0, 1.0).should ==
      Time.send(@method, 2000, 1, 1, 20, 15, 1)
    Time.send(@method, 1.0, 15.0, 20.0, 1.0, 1.0, 2000.0, :ignored, :ignored,
              :ignored, :ignored).should ==
      Time.send(@method, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored)
  end

  ruby_version_is ""..."1.9.1" do
    it "accepts various year ranges" do
      Time.send(@method, 1901, 12, 31, 23, 59, 59, 0).wday.should == 2
      Time.send(@method, 2037, 12, 31, 23, 59, 59, 0).wday.should == 4

      platform_is :wordsize => 32 do
        lambda {
          Time.send(@method, 1900, 12, 31, 23, 59, 59, 0)
        }.should raise_error(ArgumentError) # mon

        lambda {
          Time.send(@method, 2038, 12, 31, 23, 59, 59, 0)
        }.should raise_error(ArgumentError) # mon
      end

      platform_is :wordsize => 64 do
        Time.send(@method, 1900, 12, 31, 23, 59, 59, 0).wday.should == 1
        Time.send(@method, 2038, 12, 31, 23, 59, 59, 0).wday.should == 5
      end
    end

    it "raises an ArgumentError for out of range values" do
      # year-based Time.local(year (, month, day, hour, min, sec, usec))
      # Year range only fails on 32 bit archs
      platform_is :wordsize => 32 do
        lambda {
          Time.send(@method, 1111, 12, 31, 23, 59, 59, 0)
        }.should raise_error(ArgumentError) # year
      end

      lambda {
        Time.send(@method, 2008, 13, 31, 23, 59, 59, 0)
      }.should raise_error(ArgumentError) # mon

      lambda {
        Time.send(@method, 2008, 12, 32, 23, 59, 59, 0)
      }.should raise_error(ArgumentError) # day

      lambda {
        Time.send(@method, 2008, 12, 31, 25, 59, 59, 0)
      }.should raise_error(ArgumentError) # hour

      lambda {
        Time.send(@method, 2008, 12, 31, 23, 61, 59, 0)
      }.should raise_error(ArgumentError) # min

      lambda {
        Time.send(@method, 2008, 12, 31, 23, 59, 61, 0)
      }.should raise_error(ArgumentError) # sec

      # second based Time.local(sec, min, hour, day, month, year, wday, yday, isdst, tz)
      lambda {
        Time.send(@method, 61, 59, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # sec

      lambda {
        Time.send(@method, 59, 61, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # min

      lambda {
        Time.send(@method, 59, 59, 25, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # hour

      lambda {
        Time.send(@method, 59, 59, 23, 32, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # day

      lambda {
        Time.send(@method, 59, 59, 23, 31, 13, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # month

      # Year range only fails on 32 bit archs
      platform_is :wordsize => 32 do
        lambda {
          Time.send(@method, 59, 59, 23, 31, 12, 1111, :ignored, :ignored, :ignored, :ignored)
        }.should raise_error(ArgumentError) # year
      end
    end
  end

  # MRI 1.9 relaxes 1.8's restriction's on allowed years.
  ruby_version_is "1.9" do
    it "accepts various year ranges" do
      Time.send(@method, 1801, 12, 31, 23, 59, 59, 0).wday.should == 4
      Time.send(@method, 3000, 12, 31, 23, 59, 59, 0).wday.should == 3
    end

    it "raises an ArgumentError for out of range values" do
      # year-based Time.local(year (, month, day, hour, min, sec, usec))
      # Year range only fails on 32 bit archs
      platform_is :wordsize => 32 do
      end

      lambda {
        Time.send(@method, 2008, 13, 31, 23, 59, 59, 0)
      }.should raise_error(ArgumentError) # mon

      lambda {
        Time.send(@method, 2008, 12, 32, 23, 59, 59, 0)
      }.should raise_error(ArgumentError) # day

      lambda {
        Time.send(@method, 2008, 12, 31, 25, 59, 59, 0)
      }.should raise_error(ArgumentError) # hour

      lambda {
        Time.send(@method, 2008, 12, 31, 23, 61, 59, 0)
      }.should raise_error(ArgumentError) # min

      lambda {
        Time.send(@method, 2008, 12, 31, 23, 59, 61, 0)
      }.should raise_error(ArgumentError) # sec

      # second based Time.local(sec, min, hour, day, month, year, wday, yday, isdst, tz)
      lambda {
        Time.send(@method, 61, 59, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # sec

      lambda {
        Time.send(@method, 59, 61, 23, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # min

      lambda {
        Time.send(@method, 59, 59, 25, 31, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # hour

      lambda {
        Time.send(@method, 59, 59, 23, 32, 12, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # day

      lambda {
        Time.send(@method, 59, 59, 23, 31, 13, 2008, :ignored, :ignored, :ignored, :ignored)
      }.should raise_error(ArgumentError) # month
    end
  end

  it "raises ArgumentError for invalid number of arguments" do
    # Time.local only takes either 1-8, or 10 arguments
    lambda {
      Time.send(@method, 59, 1, 2, 3, 4, 2008, 0, 0, 0)
    }.should raise_error(ArgumentError) # 9 go boom

    # please stop using should_not raise_error... it is implied
    Time.send(@method, 2008).wday.should == 2
    Time.send(@method, 2008, 12).wday.should == 1
    Time.send(@method, 2008, 12, 31).wday.should == 3
    Time.send(@method, 2008, 12, 31, 23).wday.should == 3
    Time.send(@method, 2008, 12, 31, 23, 59).wday.should == 3
    Time.send(@method, 2008, 12, 31, 23, 59, 59).wday.should == 3
    Time.send(@method, 2008, 12, 31, 23, 59, 59, 0).wday.should == 3
    Time.send(@method, 59, 1, 2, 3, 4, 2008, :x, :x, :x, :x).wday.should == 4
  end

  it "accepts a String or Integer only for arguments" do
    Time.send(@method, "0").should == Time.send(@method, 0)

    m = mock(:int)
    m.should_receive(:to_int).and_return(0)

    Time.send(@method, m).should == Time.send(@method, 0)
  end

  it "does not accept nil for params" do
    lambda {
      Time.send(@method, nil)
    }.should raise_error(TypeError)
  end

  it "returns subclass instances" do
    c = Class.new(Time)
    c.send(@method, 2008, "12").should be_kind_of(c)
  end
end
