@dir_pwd = shared "Dir.pwd" do |cmd|
  describe "Dir.pwd" do
    it "returns the current working directory" do
      Dir.send(cmd).should == `pwd`.chomp
    end
  end
end
