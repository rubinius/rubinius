require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
module Runner
module Formatter
context "SpecdocFormatter" do
    setup do
        @io = StringIO.new
        @formatter = SpecdocFormatter.new(@io)
      
    end
    specify "should produce standard summary" do
        @formatter.dump_summary(3, 2, 1)
        @io.string.should_eql("\nFinished in 3 seconds\n\n2 specifications, 1 failure\n")
      
    end
    specify "should push context name" do
        @formatter.add_context("context", :ignored)
        @io.string.should_eql("\ncontext\n")
      
    end
    specify "should push failing spec name and failure number" do
        @formatter.spec_failed("spec", 98, Reporter::Failure.new("c", "s", RuntimeError.new))
        @io.string.should_eql("- spec (ERROR - 98)\n")
      
    end
    specify "should push nothing on start" do
        @formatter.start(5)
        @io.string.should_eql("")
      
    end
    specify "should push nothing on start dump" do
        @formatter.start_dump
        @io.string.should_eql("")
      
    end
    specify "should push passing spec name" do
        @formatter.spec_passed("spec")
        @io.string.should_eql("- spec\n")
      
    end
  
end
end
end
end