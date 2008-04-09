shared :env_each do |cmd|
  describe "ENV##{cmd}" do

    it "returns each pair" do
      e = []
      ENV.send(cmd) { |k, v| e << [k, v] }
      env = `env`.split("\n").map { |l| [l.gsub(/=.*$/, ''), l.gsub(/^.*=/,'')] }
      e.size.should == env.size
      e.sort == env.sort
    end

    it "raises LocalJumpError if no block given" do
      lambda { ENV.each_key }.should raise_error(LocalJumpError)
    end

  end
end
