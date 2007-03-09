require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Runner
    module Formatter
      context "ProgressBarFormatter" do
        setup do
          @io = StringIO.new
          @formatter = ProgressBarFormatter.new(@io)
        end

        specify "should produce line break on start dump" do
          @formatter.start_dump
          @io.string.should_eql("\n")
        end

        specify "should produce standard summary" do
          @formatter.dump_summary(3, 2, 1)
          @io.string.should_eql("\nFinished in 3 seconds\n\n2 specifications, 1 failure\n")
        end

        specify "should push F for failing spec" do
          @formatter.spec_failed("spec", 98, Reporter::Failure.new("c", "s", RuntimeError.new))
          @io.string.should_eql("F")
        end

        specify "should push dot for passing spec" do
          @formatter.spec_passed("spec")
          @io.string.should_eql(".")
        end

        specify "should push line break for context" do
          @formatter.add_context("context", :ignored)
          @io.string.should_eql("\n")
        end

        specify "should push nothing on start" do
          @formatter.start(4)
          @io.string.should_eql("")
        end

        specify "should ensure two ':' in the first backtrace" do
          backtrace = ["/tmp/x.rb:1", "/tmp/x.rb:2", "/tmp/x.rb:3"]
          @formatter.format_backtrace(backtrace).should_eql(<<-EOE.rstrip)
/tmp/x.rb:1:
/tmp/x.rb:2:
/tmp/x.rb:3:
EOE

          backtrace = ["/tmp/x.rb:1: message", "/tmp/x.rb:2", "/tmp/x.rb:3"]
          @formatter.format_backtrace(backtrace).should_eql(<<-EOE.rstrip)
/tmp/x.rb:1: message
/tmp/x.rb:2:
/tmp/x.rb:3:
EOE
        end
      end
      
      context "ProgressBarFormatter outputting to custom out" do
        setup do
          @out = mock("out")
          @out.stub!(:puts)
          @formatter = ProgressBarFormatter.new(@out)
          @formatter.class.send :public, :output_to_tty?
        end

        teardown do
          @formatter.class.send :protected, :output_to_tty?
        end

        specify "should not throw NoMethodError on output_to_tty?" do
          @out.should_receive(:tty?).and_raise(NoMethodError)
          @formatter.output_to_tty?.should be(false)
        end
      end
    end
  end
end