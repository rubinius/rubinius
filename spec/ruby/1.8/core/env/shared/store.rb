shared :env_store do |cmd|
  describe "ENV#[]=" do
    it "set the environment variable to the given value" do
      ENV["foo"] = "bar"
      `env | egrep '^foo='`.strip.should == "foo=bar"
    end
  end
end
