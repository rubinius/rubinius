shared :env_length do |cmd|
  describe "ENV.#{cmd}" do
    it "returns the number of ENV entries" do
      orig = ENV.to_hash
      begin
        ENV.clear
        ENV["foo"] = "bar"
        ENV["baz"] = "boo"
        ENV.send(cmd).should == 2
      ensure
        ENV.replace orig
      end
    end
  end
end
