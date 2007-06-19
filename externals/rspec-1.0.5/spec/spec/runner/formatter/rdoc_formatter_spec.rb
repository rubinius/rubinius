require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
module Runner
module Formatter
describe "RdocFormatter" do
    before(:each) do
        @io = StringIO.new
        @formatter = RdocFormatter.new(@io)
        @formatter.dry_run = true
    end
    it "should produce no summary" do
        @formatter.dump_summary(nil, nil, nil, nil)
        @io.string.should be_empty
      
    end
    it "should produce nothing on start dump" do
        @formatter.start_dump
        @io.string.should be_empty
      
    end
    it "should push out context" do
        @formatter.add_behaviour("context")
        @io.string.should eql("# context\n")
      
    end
    it "should push out failed spec" do
        @formatter.example_failed("spec", 98, nil)
        @io.string.should eql("# * spec [98 - FAILED]\n")
      
    end
    it "should push out spec" do
        @formatter.example_passed("spec")
        @io.string.should eql("# * spec\n")
      
    end

    it "should push out not implemented spec" do
        @formatter.example_not_implemented("spec")
        @io.string.should eql("# * spec [NOT IMPLEMENTED]\n")      
    end
  
end
end
end
end
