@regexp_match = shared "Regexp#=~" do |cmd|
  describe "Regexp##{cmd}" do
    it "returns the first position of the match" do
      (/(.)(.)(.)/ =~ "abc").should == 0
      $~.begin(0).should == 0
    end

    it "returns nil if there is no match" do
      /xyz/ =~ "abxyc"
      $~.should == nil
    end
  end
end
