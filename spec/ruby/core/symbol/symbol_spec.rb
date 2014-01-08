require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Symbol" do
    it "includes Comparable" do
      Symbol.include?(Comparable).should == true
    end
  end
end
