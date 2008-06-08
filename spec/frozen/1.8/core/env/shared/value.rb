shared :env_value do |cmd|
  describe "ENV.#{cmd}" do

    it "returns true if ENV has the value" do
      ENV["foo"] = "bar"
      ENV.send(cmd, "bar").should == true
      ENV["foo"] = nil
    end

    it "returns false if ENV doesn't have the value" do
      ENV.send(cmd, "this_value_should_never_exist").should == false
    end

  end
end
