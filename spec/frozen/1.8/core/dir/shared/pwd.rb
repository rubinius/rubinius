shared :dir_pwd do |cmd|
  describe "Dir.pwd" do
    it "returns the current working directory" do
      # On ubuntu gutsy, for example, /bin/pwd does not
      # understand -P. With just `pwd -P`, /bin/pwd is run.
      
      # The following uses inode rather than file names to account for
      # case insensitive file systems like default OS/X file systems
      File.stat(Dir.send(cmd)).ino.should == File.stat(`/bin/sh -c "pwd -P"`.chomp).ino
    end
  end
end
