require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    describe BehaviourRunner do

      it "should only run behaviours with at least one example" do
        desired_context = mock("desired context")
        desired_context.should_receive(:run)
        desired_context.should_receive(:retain_examples_matching!)
        desired_context.should_receive(:number_of_examples).twice.and_return(1)

        other_context = mock("other context")
        other_context.should_receive(:run).never
        other_context.should_receive(:retain_examples_matching!)
        other_context.should_receive(:number_of_examples).and_return(0)

        reporter = mock("reporter")
        options = Options.new
        options.reporter = reporter
        options.examples = ["desired context legal spec"]

        runner = Spec::Runner::BehaviourRunner.new(options)
        runner.add_behaviour(desired_context)
        runner.add_behaviour(other_context)
        reporter.should_receive(:start)
        reporter.should_receive(:end)
        reporter.should_receive(:dump)
        runner.run([], false)
      end

      it "should dump even if Interrupt exception is occurred" do
        behaviour = Spec::DSL::Behaviour.new("context") do
          it "no error" do
          end

          it "should interrupt" do
            raise Interrupt
          end
        end
        
        reporter = mock("reporter")
        reporter.should_receive(:start)
        reporter.should_receive(:add_behaviour).with("context")
        reporter.should_receive(:example_started).twice
        reporter.should_receive(:example_finished).twice
        reporter.should_receive(:rspec_verify)
        reporter.should_receive(:rspec_reset)
        reporter.should_receive(:end)
        reporter.should_receive(:dump)

        options = Options.new
        options.reporter = reporter
        runner = Spec::Runner::BehaviourRunner.new(options)
        runner.add_behaviour(behaviour)
        runner.run([], false)
      end

      it "should heckle when options have heckle_runner" do
        context = mock("context", :null_object => true)
        context.should_receive(:number_of_examples).twice.and_return(1)
        context.should_receive(:run).and_return(0)

        reporter = mock("reporter")
        reporter.should_receive(:start).with(1)
        reporter.should_receive(:end)
        reporter.should_receive(:dump).and_return(0)

        heckle_runner = mock("heckle_runner")
        heckle_runner.should_receive(:heckle_with)

        options = Options.new
        options.reporter = reporter
        options.heckle_runner = heckle_runner

        runner = Spec::Runner::BehaviourRunner.new(options)
        runner.add_behaviour(context)
        runner.run([], false)
      end

      it "should run specs backward if options.reverse is true" do
        options = Options.new
        options.reverse = true

        reporter = mock("reporter")
        reporter.should_receive(:start).with(3)
        reporter.should_receive(:end)
        reporter.should_receive(:dump).and_return(0)
        options.reporter = reporter

        runner = Spec::Runner::BehaviourRunner.new(options)
        c1 = mock("c1")
        c1.should_receive(:number_of_examples).twice.and_return(1)

        c2 = mock("c2")
        c2.should_receive(:number_of_examples).twice.and_return(2)
        c2.should_receive(:run) do
          c1.should_receive(:run)
        end

        runner.add_behaviour(c1)
        runner.add_behaviour(c2)
    
        runner.run([], false)
      end
      
      it "should yield global configuration" do
        Spec::Runner.configure do |config|
          config.should equal(Spec::Runner.configuration)
        end
      end
    end
  end
end
