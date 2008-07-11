shared :cgi_query_extension_has_key_p do |cmd|
  describe "CGI::QueryExtension##{cmd} when passed key" do
    before(:each) do
      ENV['REQUEST_METHOD'], @old_request_method = "GET", ENV['REQUEST_METHOD']
      ENV['QUERY_STRING'], @old_query_string = "one=a&two=b", ENV['QUERY_STRING']
      
      @cgi = CGI.new
    end

    after(:each) do
      ENV['REQUEST_METHOD'] = @old_request_method
      ENV['QUERY_STRING']   = @old_query_string
    end

    it "returns true when the passed key exists in the HTTP Query" do
      @cgi.send(cmd, "one").should be_true
      @cgi.send(cmd, "two").should be_true
      @cgi.send(cmd, "three").should be_false
    end
  end
end