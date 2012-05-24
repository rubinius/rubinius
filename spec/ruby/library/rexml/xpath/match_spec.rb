require File.expand_path('../../../../spec_helper', __FILE__)
require 'rexml/document'
require 'rexml/element'

ruby_version_is ""..."1.9" do

  describe "REXML::XPath.match" do
    before :each do
      @body = "<testsuite tests='5' skipped='0' failures='0'>test</testsuite>"
      @xpath = "/testsuite[@tests='5' and @skipped='0' and @failures='0']"
      @document = REXML::Document.new(@body)
    end

    it "should not raise exception when xpath include and more than one time" do
      lambda{
        REXML::XPath.match(@document, @xpath)
      }.should_not raise_error(NoMethodError)
    end

    it "should return one node" do
      nodes = REXML::XPath.match(@document, @xpath)
      nodes.count.should == 1
    end

    it "should return proper node" do
      nodes = REXML::XPath.match(@document, @xpath)
      nodes.first.text.should == "test"
    end
  end

end