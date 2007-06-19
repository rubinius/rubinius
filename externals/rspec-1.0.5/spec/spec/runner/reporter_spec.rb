require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    
    module ReporterSpecHelper
      def setup
        @io = StringIO.new
        @backtrace_tweaker = stub("backtrace tweaker", :tweak_backtrace => nil)
        @formatter = mock("formatter")
        @reporter = Reporter.new([@formatter], @backtrace_tweaker)
      end

      def failure
        Mocks::DuckTypeArgConstraint.new(:header, :exception)
      end
    end
    
    describe Reporter do
      include ReporterSpecHelper
      before(:each) {setup}
      
      it "should tell formatter when behaviour is added" do
        @formatter.should_receive(:add_behaviour).with("behaviour")
        @reporter.add_behaviour("behaviour")
      end

      it "should handle multiple behaviours with same name" do
        @formatter.should_receive(:add_behaviour).exactly(3).times
        @formatter.should_receive(:example_started).exactly(3).times
        @formatter.should_receive(:example_passed).exactly(3).times
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary).with(anything(), 3, 0, 0)
        @reporter.add_behaviour("behaviour")
        @reporter.example_started("spec 1")
        @reporter.example_finished("spec 1")
        @reporter.add_behaviour("behaviour")
        @reporter.example_started("spec 2")
        @reporter.example_finished("spec 2")
        @reporter.add_behaviour("behaviour")
        @reporter.example_started("spec 3")
        @reporter.example_finished("spec 3")
        @reporter.dump
      end

      it "should handle multiple examples with the same name" do
        error=RuntimeError.new
        @formatter.should_receive(:add_behaviour).exactly(2).times
        @formatter.should_receive(:example_passed).with("example").exactly(2).times
        @formatter.should_receive(:example_failed).with("example", 1, failure)
        @formatter.should_receive(:example_failed).with("example", 2, failure)
        @formatter.should_receive(:dump_failure).exactly(2).times
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary).with(anything(), 4, 2, 0)
        @backtrace_tweaker.should_receive(:tweak_backtrace).twice
        @reporter.add_behaviour("behaviour")
        @reporter.example_finished("example")
        @reporter.example_finished("example", error)
        @reporter.add_behaviour("behaviour")
        @reporter.example_finished("example")
        @reporter.example_finished("example", error)
        @reporter.dump
      end

      it "should push stats to formatter even with no data" do
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary).with(anything(), 0, 0, 0)
        @reporter.dump
      end
      
      it "should push time to formatter" do
        @formatter.should_receive(:start).with(5)
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary) do |time, a, b|
          time.to_s.should match(/[0-9].[0-9|e|-]+/)
        end
        @reporter.start(5)
        @reporter.end
        @reporter.dump
      end
    end
    
    describe Reporter, " reporting one passing example" do
      include ReporterSpecHelper
      before(:each) {setup}

      it "should tell formatter example passed" do
        @formatter.should_receive(:example_passed)
        @reporter.example_finished("example")
      end
      
      it "should not delegate to backtrace tweaker" do
        @formatter.should_receive(:example_passed)
        @backtrace_tweaker.should_not_receive(:tweak_backtrace)
        @reporter.example_finished("example")
      end

      it "should account for passing example in stats" do
        @formatter.should_receive(:example_passed)
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary).with(anything(), 1, 0, 0)
        @reporter.example_finished("example")
        @reporter.dump
      end
    end

    describe Reporter, " reporting one failing example" do
      include ReporterSpecHelper
      before(:each) {setup}

      it "should tell formatter that example failed" do
        @formatter.should_receive(:example_failed)
        @reporter.example_finished("example", RuntimeError.new)
      end
      
      it "should delegate to backtrace tweaker" do
        @formatter.should_receive(:example_failed)
        @backtrace_tweaker.should_receive(:tweak_backtrace)
        @reporter.example_finished("spec", RuntimeError.new)
      end

      it "should account for failing example in stats" do
        @formatter.should_receive(:add_behaviour)
        @formatter.should_receive(:example_failed).with("example", 1, failure)
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_failure).with(1, anything())
        @formatter.should_receive(:dump_summary).with(anything(), 1, 1, 0)
        @reporter.add_behaviour("behaviour")
        @reporter.example_finished("example", RuntimeError.new)
        @reporter.dump
      end
      
    end
    
    describe Reporter, " reporting one not implemented example" do
      include ReporterSpecHelper
      before(:each) {setup}

      it "should tell formatter example passed" do
        @formatter.should_receive(:example_not_implemented)
        @reporter.example_finished("example", nil, nil, true)
      end

      it "should account for not implemented example in stats" do
        @formatter.should_receive(:example_not_implemented)
        @formatter.should_receive(:start_dump)
        @formatter.should_receive(:dump_summary).with(anything(), 1, 0, 1)
        @reporter.example_finished("example", nil, nil, true)
        @reporter.dump
      end
    end
  end
end
