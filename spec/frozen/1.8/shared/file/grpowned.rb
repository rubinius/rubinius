shared :file_grpowned do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    
    before :each do
      @file = tmp('i_exist')
      File.open(@file,'w'){|f| f.write 'rubinius'}
      File.chown(nil, Process.gid, @file)
    end

    after :each do
      File.delete(@file) if File.exist?(@file)
    end
    
    platform_is_not :windows do
      it "return true if the file exist" do
        klass.send(cmd, @file).should == true
      end
    end

    platform_is :windows do
      it "return false if the file exist" do
        klass.send(cmd, @file).should == false
      end
    end
  end
end
