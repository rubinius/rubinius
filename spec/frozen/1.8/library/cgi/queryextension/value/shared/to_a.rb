shared :cgi_query_extension_value_to_a do |cmd|
  describe "CGI::QueryExtension::Value##{cmd}" do
    before(:each) do
      @value = "some value"
      @value.extend(CGI::QueryExtension::Value)
    end

    it "returns an Array containing self" do
      @value.send(cmd).should == [@value]
    end
  end

  describe "CGI::QueryExtension::Value##{cmd} when alternative values for self are set" do
    before(:each) do
      @value = "some value"
      @value.extend(CGI::QueryExtension::Value)
      @value.set_params(["some value", "other value", "third value"])
    end

    it "returns an array containing the alternative values" do
      @value.send(cmd).should == ["some value", "other value", "third value"]
    end
  end
end