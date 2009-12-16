describe :inspect, :shared => true do
  ruby_version_is ""..."1.9" do
    it "formats the time following the pattern 'EEE MMM dd HH:mm:ss Z yyyy'" do
      with_timezone("PST", +1) do
        Time.local("1", "15", "20", "1", "1", "2000", :ignored, :ignored, :ignored, :ignored).send(@method).should == "Sat Jan 01 20:15:01 +0100 2000"
      end
    end

    it "formats the UTC time following the pattern 'EEE MMM dd HH:mm:ss UTC yyyy'" do
      Time.utc("1", "15", "20", "1", "1", "2000", :ignored, :ignored, :ignored, :ignored).send(@method).should == "Sat Jan 01 20:15:01 UTC 2000"
    end
  end

  ruby_version_is "1.9" do
    it "formats the time following the pattern 'yyyy-MM-dd HH:mm:ss Z'" do
      with_timezone("PST", +1) do
        Time.local("1", "15", "20", "1", "1", "2000", :ignored, :ignored, :ignored, :ignored).send(@method).should == "2000-01-01 20:15:01 +0100"
      end
    end

    it "formats the UTC time following the pattern 'yyyy-MM-dd HH:mm:ss UTC'" do
      Time.utc("1", "15", "20", "1", "1", "2000", :ignored, :ignored, :ignored, :ignored).send(@method).should == "2000-01-01 20:15:01 UTC"
    end
  end
end
