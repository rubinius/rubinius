shared :file_writable_real do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do
      @file = tmp('i_exist')
    end

    after :each do
      File.delete(@file) if File.exist?(@file)
    end

    it "returns true if named file is writable by the real user id of the process, otherwise false" do
      File.open(@file,'w') { klass.send(cmd, @file).should == true }
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { File.writable_real?        }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, 1)     }.should raise_error(TypeError)
      lambda { klass.send(cmd, nil)   }.should raise_error(TypeError)
      lambda { klass.send(cmd, false) }.should raise_error(TypeError)
    end 
  end
end

shared :file_writable_real_missing do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    it "returns false if the file does not exist" do
      klass.send(cmd, 'fake_file').should == false
    end
  end
end
