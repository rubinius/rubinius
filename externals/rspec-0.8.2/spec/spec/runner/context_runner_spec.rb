require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "ContextRunner" do
  specify "should call run on context" do
    context1 = mock("context1", {
     :null_object => true
    })
    context2 = mock("context2", {
     :null_object => true
    })
    context1.should_receive(:run)
    context1.should_receive(:number_of_specs).and_return(0)
    context2.should_receive(:run)
    context2.should_receive(:number_of_specs).and_return(0)
    reporter = mock("reporter")
    reporter.should_receive(:start).with(0)
    reporter.should_receive(:end)
    reporter.should_receive(:dump)

    options = OpenStruct.new
    options.reporter = reporter
    runner = Spec::Runner::ContextRunner.new(options)
    runner.add_context(context1)
    runner.add_context(context2)
    runner.run(false)
  end

  specify "should support single spec" do
    desired_context = mock("desired context")
    desired_context.should_receive(:matches?).at_least(:once).and_return(true)
    desired_context.should_receive(:run)
    desired_context.should_receive(:run_single_spec)
    desired_context.should_receive(:number_of_specs).and_return(1)
    other_context = mock("other context")
    other_context.should_receive(:matches?).and_return(false)
    other_context.should_receive(:run).never
    other_context.should_receive(:number_of_specs).never
    reporter = mock("reporter")
    options = OpenStruct.new
    options.reporter = reporter
    options.spec_name = "desired context legal spec"

    runner = Spec::Runner::ContextRunner.new(options)
    runner.add_context(desired_context)
    runner.add_context(other_context)
    reporter.should_receive(:start)
    reporter.should_receive(:end)
    reporter.should_receive(:dump)
    runner.run(false)
  end

  specify "should dump even if Interrupt exception is occurred" do
    context = Spec::Runner::Context.new("context") do
      specify "no error" do
      end

      specify "should interrupt" do
        raise Interrupt
      end
    end

    reporter = mock("reporter")
    reporter.should_receive(:start)
    reporter.should_receive(:add_context).with("context")
    reporter.should_receive(:spec_started).with("no error")
    reporter.should_receive(:spec_started).with("should interrupt")
    reporter.should_receive(:spec_finished).twice
    reporter.should_receive(:end)
    reporter.should_receive(:dump)

    options = OpenStruct.new
    options.reporter = reporter
    runner = Spec::Runner::ContextRunner.new(options)
    runner.add_context(context)
    runner.run(false)
  end

  specify "should heckle when options have heckle_runner" do
    context = mock("context", :null_object => true)
    context.should_receive(:number_of_specs).and_return(0)
    context.should_receive(:run).and_return(0)

    reporter = mock("reporter")
    reporter.should_receive(:start).with(0)
    reporter.should_receive(:end)
    reporter.should_receive(:dump).and_return(0)

    heckle_runner = mock("heckle_runner")
    heckle_runner.should_receive(:heckle_with)

    options = OpenStruct.new
    options.reporter = reporter
    options.heckle_runner = heckle_runner

    runner = Spec::Runner::ContextRunner.new(options)
    runner.add_context(context)
    runner.run(false)

  end
end
