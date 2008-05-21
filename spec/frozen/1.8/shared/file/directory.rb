shared :file_directory do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do
      platform_is :windows do
        @dir  = "C:\\"
        @file = "C:\\winnt\\notepad.exe"
      end

      platform_is_not :windows do
        @dir  = "/"
        @file = "/bin/ls"
      end
    end

    after :each do
      @dir = nil
    end

    it "return true if dir is a directory, otherwise return false" do
      klass.send(cmd, @dir).should == true
      klass.send(cmd, @file).should == false
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { klass.send(cmd)              }.should raise_error(ArgumentError)
      lambda { klass.send(cmd, @dir, @file) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, nil) }.should raise_error(TypeError)
    end
  end
end
