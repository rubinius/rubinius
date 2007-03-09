require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
module Runner
module Formatter
context "RdocFormatter" do
    setup do
        @io = StringIO.new
        @formatter = RdocFormatter.new(@io, true)
      
    end
    specify "should produce no summary" do
        @formatter.dump_summary(nil, nil, nil)
        @io.string.should_be_empty
      
    end
    specify "should produce nothing on start dump" do
        @formatter.start_dump
        @io.string.should_be_empty
      
    end
    specify "should push out context" do
        @formatter.add_context("context", :ignored)
        @io.string.should_eql("# context\n")
      
    end
    specify "should push out failed spec" do
        @formatter.spec_failed("spec", 98, nil)
        @io.string.should_eql("# * spec [98 - FAILED]\n")
      
    end
    specify "should push out spec" do
        @formatter.spec_passed("spec")
        @io.string.should_eql("# * spec\n")
      
    end
  
end
end
end
end