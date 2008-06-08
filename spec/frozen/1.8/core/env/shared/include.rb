shared :env_include do |cmd|
  describe "ENV.#{cmd}" do

    it "returns true if ENV has the key" do
      ENV["foo"] = "bar"
      ENV.send(cmd, "foo").should == true
      ENV.delete "foo"
    end

    it "return false if ENV doesn't include the key" do
      ENV.send(cmd, "should_never_be_set").should == false
    end

  end
end
