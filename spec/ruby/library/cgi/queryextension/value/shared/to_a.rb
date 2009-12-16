describe :cgi_query_extension_value_to_a, :shared => true do
  before(:each) do
    @value = "some value"
    @value.extend(CGI::QueryExtension::Value)
  end

  it "returns an Array containing self" do
    @value.send(@method).should == [@value]
  end
end

describe :cgi_query_extension_value_to_a_alternate_self, :shared => true do
  before(:each) do
    @value = "some value"
    @value.extend(CGI::QueryExtension::Value)
    @value.set_params(["some value", "other value", "third value"])
  end

  it "returns an array containing the alternative values" do
    @value.send(@method).should == ["some value", "other value", "third value"]
  end
end
