shared :net_http_version_1_2_p do |cmd|
  describe "Net::HTTP.#{cmd}" do
    it "returns the state of net/http 1.2 features" do
      Net::HTTP.version_1_1
      Net::HTTP.send(cmd).should be_false
      Net::HTTP.version_1_2
      Net::HTTP.send(cmd).should be_true
    end
  end
end