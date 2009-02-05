describe :net_httpheader_set_form_data, :shared => true do
  before(:each) do
    @headers = NetHTTPHeaderSpecs::Example.new
  end

  describe "when passed params" do
    it "automatically set the 'Content-Type' to 'application/x-www-form-urlencoded'" do
      @headers.send(@method, "cmd" => "search", "q" => "ruby", "max" => "50")
      @headers["Content-Type"].should == "application/x-www-form-urlencoded"
    end
    
    it "sets self's body based on the passed form parameters" do
      @headers.send(@method, "cmd" => "search", "q" => "ruby", "max" => "50")
      @headers.body.split("&").should == ["max=50", "cmd=search", "q=ruby"]
    end
  end

  describe "when passed params, separator" do
    it "sets self's body based on the passed form parameters and the passed separator" do
      @headers.send(@method, {"cmd" => "search", "q" => "ruby", "max" => "50"}, "&")
      @headers.body.split("&").should == ["max=50", "cmd=search", "q=ruby"]

      @headers.send(@method, {"cmd" => "search", "q" => "ruby", "max" => "50"}, ";")
      @headers.body.split(";").should == ["max=50", "cmd=search", "q=ruby"]
    end
  end
end