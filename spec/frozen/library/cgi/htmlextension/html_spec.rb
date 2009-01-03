require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/fixtures/common"

describe "CGI::HtmlExtension#html" do
  before(:each) do
    @html = CGISpecs::HtmlExtension.new
    @html.stub!(:doctype).and_return("<!DOCTYPE SUPA-FUNKAY-RUBYSPEC-DOCTYPE>")
  end

  describe "when passed no arguments" do
    it "returns a self's doctype and an 'html'-element" do
      expected = '<!DOCTYPE SUPA-FUNKAY-RUBYSPEC-DOCTYPE><HTML>'
      @html.html.should == expected
    end
  
    it "includes the passed block when passed a block" do
      expected = '<!DOCTYPE SUPA-FUNKAY-RUBYSPEC-DOCTYPE><HTML>test</HTML>'
      @html.html { "test" }.should == expected
    end
  end

  describe "when passed 'PRETTY'" do
    it "returns pretty output when the passed String is 'PRETTY" do
      expected = "<!DOCTYPE SUPA-FUNKAY-RUBYSPEC-DOCTYPE>\n<HTML>\n"
      @html.html("PRETTY").should == expected
    end

    it "includes the passed block when passed a block" do
      expected = "<!DOCTYPE SUPA-FUNKAY-RUBYSPEC-DOCTYPE>\n<HTML>\n  test\n</HTML>\n"
      @html.html("PRETTY") { "test" }.should == expected
    end
  end

  describe "when passed a Hash" do
    it "returns an 'html'-element using the passed Hash for attributes" do
      expected = '<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"><HTML BLA="TEST">'
      @html.html("DOCTYPE" => '<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">', "BLA" => "TEST").should == expected
    end

    it "omits the doctype when the Hash contains a 'DOCTYPE' entry that's false or nil" do
      @html.html("DOCTYPE" => false).should == "<HTML>"
      @html.html("DOCTYPE" => nil).should == "<HTML>"
    end
  end
end
