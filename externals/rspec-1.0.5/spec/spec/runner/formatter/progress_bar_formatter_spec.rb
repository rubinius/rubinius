require File.dirname(__FILE__) + '/../../../spec_helper.rb'

module Spec
  module Runner
    module Formatter
      describe "ProgressBarFormatter" do
        before(:each) do
          @io = StringIO.new
          @formatter = ProgressBarFormatter.new(@io)
        end

        it "should produce line break on start dump" do
          @formatter.start_dump
          @io.string.should eql("\n")
        end

        it "should produce standard summary without not implemented when not implemented has a 0 count" do
          @formatter.dump_summary(3, 2, 1, 0)
          @io.string.should eql("\nFinished in 3 seconds\n\n2 examples, 1 failure\n")
        end
        
        it "should produce standard summary" do
          @formatter.dump_summary(3, 2, 1, 4)
          @io.string.should eql("\nFinished in 3 seconds\n\n2 examples, 1 failure, 4 not implemented\n")
        end

        it "should push F for failing spec" do
          @formatter.example_failed("spec", 98, Reporter::Failure.new("c s", RuntimeError.new))
          @io.string.should eql("F")
        end

        it "should push dot for passing spec" do
          @formatter.example_passed("spec")
          @io.string.should eql(".")
        end

        it "should push nothing on start" do
          @formatter.start(4)
          @io.string.should eql("")
        end

        it "should ensure two ':' in the first backtrace" do
          backtrace = ["/tmp/x.rb:1", "/tmp/x.rb:2", "/tmp/x.rb:3"]
          @formatter.format_backtrace(backtrace).should eql(<<-EOE.rstrip)
/tmp/x.rb:1:
/tmp/x.rb:2:
/tmp/x.rb:3:
EOE

          backtrace = ["/tmp/x.rb:1: message", "/tmp/x.rb:2", "/tmp/x.rb:3"]
          @formatter.format_backtrace(backtrace).should eql(<<-EOE.rstrip)
/tmp/x.rb:1: message
/tmp/x.rb:2:
/tmp/x.rb:3:
EOE
        end
      end
      
      describe "ProgressBarFormatter outputting to custom out" do
        before(:each) do
          @out = mock("out")
          @out.stub!(:puts)
          @formatter = ProgressBarFormatter.new(@out)
          @formatter.class.send :public, :output_to_tty?
        end

        after(:each) do
          @formatter.class.send :protected, :output_to_tty?
        end

        it "should not throw NoMethodError on output_to_tty?" do
          @out.should_receive(:tty?).and_raise(NoMethodError)
          @formatter.output_to_tty?.should be_false
        end
      end
    end
  end
end
