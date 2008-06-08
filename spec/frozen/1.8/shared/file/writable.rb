shared :file_writable do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do
      @file = tmp('i_exist')
    end

    after :each do
      File.delete(@file) if File.exists?(@file)
    end

    it "returns true if named file is writable by the effective user id of the process, otherwise false" do
      klass.send(cmd, '/etc/passwd').should == false
      File.open(@file,'w') { klass.send(cmd, @file).should == true }
    end
  end
end

shared :file_writable_missing do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    it "returns false if the file does not exist" do
      klass.send(cmd, 'fake_file').should == false
    end
  end
end
