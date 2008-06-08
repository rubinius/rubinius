shared :file_zero do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do
      @zero_file    = 'test.txt'
      @nonzero_file = 'test2.txt'

      File.open(@zero_file, "w") {} # Touch
      File.open(@nonzero_file, "w") { |f| f.puts "hello" }
    end

    after :each do
      File.delete(@zero_file) if File.exists?(@zero_file)
      File.delete(@nonzero_file) if File.exists?(@nonzero_file)  
      @zero_file    = nil
      @nonzero_file = nil
    end

    it "return true if the length of a file its zero, otherwise true" do
      klass.send(cmd, @zero_file).should == true
      klass.send(cmd, @nonzero_file).should == false
    end

    platform_is :windows do
      it "returns true for NUL" do
        klass.send(cmd, 'NUL').should == true
      end
    end

    platform_is_not :windows do
      it "returns true for /dev/null" do
        klass.send(cmd, '/dev/null').should == true
      end
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { File.zero?        }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, nil)   }.should raise_error(TypeError)
      lambda { klass.send(cmd, true)  }.should raise_error(TypeError)
      lambda { klass.send(cmd, false) }.should raise_error(TypeError)
    end

    it "zero? should return true if the named file exists and has a zero size." do
      begin
        file = tmp('i_exist')
        File.open(file,'w') { klass.send(cmd, file).should == true }
      ensure
        File.delete(file) rescue nil
      end
    end
  end 
end

shared :file_zero_missing do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    it "returns false if the file does not exist" do
      klass.send(cmd, 'fake_file').should == false
    end
  end
end
