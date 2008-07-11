shared :cgi_query_extension_value_first do |cmd|
  describe "CGI::QueryExtension::Value##{cmd}" do
    before(:each) do
      @value = "some value"
      @value.extend(CGI::QueryExtension::Value)
    end
    
    it "returns self" do
      @value.send(cmd).should equal(@value)
    end
    
    it "outputs a warning" do
      expected_warning = /#{Regexp.quote("CAUTION! cgi['key'] == cgi.params['key'][0]; if want Array, use cgi.params['key']")}/
      lambda { @value.send(cmd) }.should complain(expected_warning)
    end
  end
end