shared :regexp_match do |cmd|
  describe "Regexp##{cmd}" do
    it "returns nil if there is no match" do
      /xyz/.send(cmd,"abxyc").should == nil
    end

    it "returns nil if the object is nil" do
      /\w+/.send(cmd, nil).should == nil
    end
  end
end
