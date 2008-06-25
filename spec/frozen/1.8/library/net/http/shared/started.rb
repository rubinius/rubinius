shared :net_http_started_p do |cmd|
  describe "Net::HTTP##{cmd}" do
    before(:all) do
      NetHTTPSpecs.start_server
    end
    
    after(:all) do
      NetHTTPSpecs.stop_server
    end

    before(:each) do
      @net = Net::HTTP.new("localhost", 3333)
    end

    it "returns true when self has been started" do
      @net.start
      @net.send(cmd).should be_true
    end

    it "returns false when self has not been started yet" do
      @net.send(cmd).should be_false
    end

    it "returns false when self has been stopped again" do
      @net.start
      @net.finish
      @net.send(cmd).should be_false
    end
  end
end