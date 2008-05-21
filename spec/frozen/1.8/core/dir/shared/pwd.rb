shared :dir_pwd do |cmd|
  describe "Dir.pwd" do
    it "returns the current working directory" do
      # On ubuntu gutsy, for example, /bin/pwd does not
      # understand -P. With just `pwd -P`, /bin/pwd is run.
      Dir.send(cmd).should == `/bin/sh -c "pwd -P"`.chomp
    end
  end
end
