shared :file_symlink do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do   
      @file = "test.txt"
      @link = "test.lnk"     
      File.delete(@link) if File.exist?(@link)
      File.delete(@file) if File.exist?(@file)
      File.open(@file,"w+") 
    end 

    after :each do
      File.unlink(@link) if File.exist?(@link)
      File.delete(@file) if File.exist?(@file)
      @link = nil
    end

    platform_is_not :windows do
      it "returns true if the file is a link" do
        File.symlink(@file, @link)
        klass.send(cmd, @link).should == true
      end
    end
  end
end
