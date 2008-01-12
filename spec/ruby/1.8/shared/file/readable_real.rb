shared :file_readable_real do |cmd, klass|
  describe "#{klass}.#{cmd}" do
    before :each do
      @file = '/tmp/i_exist'
    end

    after :each do
      File.delete(@file) if File.exists?(@file)
    end

    it "returns true if named file is readable by the real user id of the process, otherwise false" do
      klass.send(cmd, 'fake_file').should == false
      File.open(@file,'w') { klass.send(cmd, @file).should == true }
    end
  end
end
