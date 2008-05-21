shared :file_executable do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do
      @file1 = tmp('temp1.txt')
      @file2 = tmp('temp2.txt')

      File.open(@file1, "w"){} # touch
      File.open(@file2, "w"){} # touch

      File.chmod(0755, @file1)
    end

    after :each do
      File.delete(@file1) if File.exist?(@file1)
      File.delete(@file2) if File.exist?(@file2)

      @file1 = nil
      @file2 = nil
    end

    platform_is_not :windows do
      it "returns true if named file is executable by the effective user id of the process, otherwise false" do
        klass.send(cmd, '/etc/passwd').should == false
        klass.send(cmd, @file1).should == true
        klass.send(cmd, @file2).should == false
      end

      it "return true if the argument is an executable file" do
        klass.send(cmd, @file1).should == true
        klass.send(cmd, @file2).should == false
      end
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { klass.send(cmd) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, 1)     }.should raise_error(TypeError)
      lambda { klass.send(cmd, nil)   }.should raise_error(TypeError)
      lambda { klass.send(cmd, false) }.should raise_error(TypeError)
    end
  end
end

shared :file_executable_missing do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    it "returns false if the file does not exist" do
      klass.send(cmd, 'fake_file').should == false
    end
  end
end
