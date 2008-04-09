shared :env_length do |cmd|
  describe "ENV##{cmd}" do
    it "returns the number of ENV entries" do
      ENV.send(cmd).should == `env`.split("\n").size
    end
  end
end
