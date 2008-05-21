shared :file_file do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do 
      platform_is :windows do
        @null = "NUL"
        @dir  = "C:\\"
      end

      platform_is_not :windows do
        @null = "/dev/null"
        @dir  = "/bin"
      end

      @file = tmp("test.txt")
      File.open(@file, "w"){} # touch
    end

    after :each do
      File.delete(@file) rescue nil
      @null = nil
      @file = nil
    end

    it "returns true if the named file exists and is a regular file." do 
      klass.send(cmd, @file).should == true
      klass.send(cmd, @dir).should == false
    end

    platform_is_not :windows do
      it "return true if the null device exists and is a regular file." do
        klass.send(cmd, @null).should == false # May fail on MS Windows
      end
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { klass.send(cmd)               }.should raise_error(ArgumentError)
      lambda { klass.send(cmd, @null, @file) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, nil) }.should raise_error(TypeError)
      lambda { klass.send(cmd, 1)   }.should raise_error(TypeError)
    end
  end
end
