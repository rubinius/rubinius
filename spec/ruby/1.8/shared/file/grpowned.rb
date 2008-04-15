shared :file_grpowned do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    
    before :each do
      @file = '/tmp/i_exist'
      File.open(@file,'w'){|f| f.write 'rubinius'}
      File.chown(nil, Process.gid, @file)
    end

    after :each do
      File.delete(@file) if File.exist?(@file)
    end
    
    it "return true if the file exist" do
      klass.send(cmd, @file).should == true
    end
  end
end
