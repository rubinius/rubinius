require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Runner
    context "A Specification class" do
      setup do
        @reporter = mock("reporter")
        callback_container = Callback::CallbackContainer.new
        @specification = Specification.dup
        @specification.stub!(:callbacks).and_return {callback_container}
      end

      specify "should have a before_setup callback for all specs" do
        @reporter.stub!(:spec_started)
        @reporter.stub!(:spec_finished)

        before_setup_called = false
        @specification.before_setup {before_setup_called = true}

        spec = @specification.new("spec") {}
        spec.run(@reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))

        before_setup_called.should == true
      end

      specify "should have before_setup callback add errors to spec run" do
        error=Exception.new
        @specification.before_setup do
          raise(error)
        end
        spec=@specification.new("spec") {}
        @reporter.should_receive(:spec_started).with("spec")
        @reporter.should_receive(:spec_finished).with("spec", error, "setup")
        spec.run(@reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))
      end

      specify "should report exceptions in spec" do
        error = Exception.new
        spec=@specification.new("spec") do
          raise(error)
        end
        @reporter.should_receive(:spec_started).with("spec")
        @reporter.should_receive(:spec_finished).with("spec", error, "spec")
        spec.run(@reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))
      end

      specify "should have an after_teardown callback for all specs" do
        @reporter.stub!(:spec_started)
        @reporter.stub!(:spec_finished)

        after_teardown_called = false
        @specification.after_teardown {after_teardown_called = true}

        spec = @specification.new("spec") {}
        spec.run(@reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))

        after_teardown_called.should == true
      end

      specify "should have after_teardown callback add errors to spec run" do
        error=Exception.new
        @specification.after_teardown do
          raise(error)
        end
        spec=@specification.new("spec") {}
        @reporter.should_receive(:spec_started).with("spec")
        @reporter.should_receive(:spec_finished).with("spec", error, "teardown")
        spec.run(@reporter, nil, nil, nil, ::Spec::Runner::ExecutionContext.new(nil))
      end
    end
  end
end
