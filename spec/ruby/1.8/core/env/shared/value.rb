shared :env_value do |cmd|
  describe "ENV##{cmd}" do

    it "returns true if ENV has the value" do
      ENV["foo"] = "bar"
      ENV.has_value?("bar").should == true
      ENV["foo"] = nil
    end

    it "returns false if ENV doesn't have the value" do
      ENV.has_value?("this_value_should_never_exist").should == false
    end

  end
end
