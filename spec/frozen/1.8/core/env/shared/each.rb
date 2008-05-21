shared :env_each do |cmd|
  describe "ENV##{cmd}" do

    it "returns each pair" do
      orig = ENV.to_hash
      e = []
      begin
        ENV.clear
        ENV["foo"] = "bar"
        ENV["baz"] = "boo"
        ENV.send(cmd) { |k, v| e << [k, v] }
        e.should include(["foo", "bar"])
        e.should include(["baz", "boo"])
      ensure
        ENV.replace orig
      end
    end

    it "raises LocalJumpError if no block given" do
      lambda { ENV.send(cmd) }.should raise_error(LocalJumpError)
    end

  end
end
