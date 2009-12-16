require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  
  require File.dirname(__FILE__) + '/fixtures/common'

  describe "Generator#rewind" do
    it "does nothing for empty generator" do
      g = GeneratorSpecs.empty
      g.index.should == 0
      g.rewind
      g.index.should == 0
    end

    it "rewinds the generator" do
      g = GeneratorSpecs.two_elems
      orig = g.next
      g.index.should == 1
      g.rewind
      g.index.should == 0
      g.next.should == orig
    end

    it "rewinds the previously finished generator" do
      g = GeneratorSpecs.two_elems
      g.next; g.next
      g.rewind
      g.end?.should == false
      g.next?.should == true
      g.next.should == 1
    end
  end
end
