shared :iconv_initialize_exceptions do |cmd, *args|
  describe "Iconv.#{cmd}" do
    it "raises a TypeError when encoding names are not Strings or string-compatible" do
      lambda { Iconv.send cmd, Object.new, "us-ascii", *args }.should raise_error(TypeError)
      lambda { Iconv.send cmd, "us-ascii", Object.new, *args }.should raise_error(TypeError)
    end

    it "raises an Iconv::InvalidEncoding exception when an encoding cannot be found" do
      lambda { Iconv.send cmd, "x-nonexistent-encoding", "us-ascii", *args }.should raise_error(Iconv::InvalidEncoding)
    end
  end
end
