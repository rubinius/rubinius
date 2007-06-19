require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    describe "Options" do
      before do
        @error_stream = StringIO.new('')
        @out_stream = StringIO.new('')
        @options = Options.new
      end

      it "instantiates empty arrays" do
        @options.examples.should == []
        @options.formatters.should == []
      end

      it "defaults to QuietBacktraceTweaker" do
        @options.backtrace_tweaker.class.should == QuietBacktraceTweaker
      end

      it "defaults to no dry_run" do
        @options.dry_run.should == false
      end

      it "parse_diff sets context_lines" do
        @options.parse_diff nil, @out_stream, @error_stream
        @options.context_lines.should == 3
      end

      it "defaults diff to unified" do
        @options.parse_diff nil, @out_stream, @error_stream
        @options.diff_format.should == :unified
      end

      it "should use unified diff format option when format is unified" do
        @options.parse_diff 'unified', @out_stream, @error_stream
        @options.diff_format.should == :unified
        @options.differ_class.should equal(Spec::Expectations::Differs::Default)
      end

      it "should use context diff format option when format is context" do
        @options.parse_diff 'context', @out_stream, @error_stream
        @options.diff_format.should == :context
        @options.differ_class.should == Spec::Expectations::Differs::Default
      end

      it "should use custom diff format option when format is a custom format" do
        @options.parse_diff "Custom::Formatter", @out_stream, @error_stream
        @options.diff_format.should == :custom
        @options.differ_class.should == Custom::Formatter
      end

      it "should print instructions about how to fix bad differ" do
        @options.parse_diff "Custom::BadFormatter", @out_stream, @error_stream
        @error_stream.string.should match(/Couldn't find differ class Custom::BadFormatter/n)
      end      

      it "parse_example sets single example when argument not a file" do
        example = "something or other"
        File.file?(example).should == false
        @options.parse_example example
        @options.examples.should eql(["something or other"])
      end

      it "parse_example sets examples to contents of file" do
        example = "#{File.dirname(__FILE__)}/examples.txt"
        File.should_receive(:file?).with(example).and_return(true)
        file = StringIO.new("Sir, if you were my husband, I would poison your drink.\nMadam, if you were my wife, I would drink it.")
        File.should_receive(:open).with(example).and_return(file)
        
        @options.parse_example example
        @options.examples.should eql([
          "Sir, if you were my husband, I would poison your drink.",
          "Madam, if you were my wife, I would drink it."
        ])
      end
    end

    describe "Options", "receiving create_behaviour_runner" do
      before do
        @options = Options.new
      end

      it "returns nil when generate is true" do
        @options.generate = true
        @options.create_behaviour_runner.should == nil
      end

      it "returns a BehaviourRunner by default" do
        runner = @options.create_behaviour_runner
        runner.class.should == BehaviourRunner
      end

      it "returns a custom runner when runner_type is set" do
        runner_type = Class.new do
          attr_reader :options
          def initialize(options)
            @options = options
          end
        end
        @options.runner_type = runner_type

        runner = @options.create_behaviour_runner
        runner.class.should == runner_type
        runner.options.should === @options
      end

      it "does not set Expectations differ when differ_class is not set" do
        @options.differ_class = nil
        Spec::Expectations.should_not_receive(:differ=)
        @options.create_behaviour_runner
      end

      it "sets Expectations differ when differ_class is set" do
        @options.differ_class = Spec::Expectations::Differs::Default
        Spec::Expectations.should_receive(:differ=).with(anything()).and_return do |arg|
          arg.class.should == Spec::Expectations::Differs::Default
        end
        @options.create_behaviour_runner
      end

      it "creates a Reporter" do
        formatter = ::Spec::Runner::Formatter::BaseFormatter.new(:somewhere)
        @options.formatters << formatter
        reporter = Reporter.new(@formatters, @backtrace_tweaker)
        Reporter.should_receive(:new).with(@options.formatters, @options.backtrace_tweaker).and_return(reporter)
        @options.create_behaviour_runner
        @options.reporter.should === reporter
      end

      it "sets colour and dry_run on the formatters" do
        @options.colour = true
        @options.dry_run = true
        formatter = ::Spec::Runner::Formatter::BaseTextFormatter.new(:somewhere)
        formatter.should_receive(:colour=).with(true)
        formatter.should_receive(:dry_run=).with(true)
        @options.formatters << formatter
        @options.create_behaviour_runner
      end
    end
  end
end
