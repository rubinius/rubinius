require File.dirname(__FILE__) + '/../../spec_helper'

module Spec
  module DSL
    describe Behaviour, ", with :shared => true" do
      
      before do
        @formatter = Spec::Mocks::Mock.new("formatter", :null_object => true)
        @behaviour = behaviour_class.new("context") {}
      end

      after do
        @formatter.rspec_verify
        @behaviour_class = nil
        $shared_behaviours.clear unless $shared_behaviours.nil?
      end

      def behaviour_class
        unless @behaviour_class
          @behaviour_class = Behaviour.dup
          # dup copies the class instance vars
          @behaviour_class.instance_variable_set(:@shared_behaviours, nil)
        end
        @behaviour_class
      end

      def make_shared_behaviour(name, opts, &block)
        behaviour = behaviour_class.new(name, opts, &block)
        behaviour_class.add_shared_behaviour(behaviour)
        behaviour
      end

      it "should accept an optional options hash" do
        lambda { behaviour_class.new("context") {} }.should_not raise_error(Exception)
        lambda { behaviour_class.new("context", :shared => true) {} }.should_not raise_error(Exception)
      end

      it "should return all shared behaviours" do
        make_shared_behaviour("b1", :shared => true) {}
        make_shared_behaviour("b2", :shared => true) {}
        
        behaviour_class.find_shared_behaviour("b1").should_not be_nil
        behaviour_class.find_shared_behaviour("b2").should_not be_nil
      end

      it "should be shared when configured as shared" do
        behaviour = make_shared_behaviour("context", :shared => true) {}
        behaviour.should be_shared
      end

      it "should not be shared when not configured as shared" do
        @behaviour.should_not be_shared
      end

      it "should not run when shared" do
        behaviour = make_shared_behaviour("context", :shared => true) {}
        $spec_ran = false
        behaviour.it("test") {$spec_ran = true}
        behaviour.run(@formatter)
        $spec_ran.should be_false
      end

      it "should contain examples when shared" do
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.it("shared example") {}
        shared_behaviour.number_of_examples.should == 1
      end

      it "should complain when adding a shared behaviour with the same description" do
        make_shared_behaviour("shared behaviour", :shared => true) {}
        lambda { make_shared_behaviour("shared behaviour", :shared => true) {} }.should raise_error(ArgumentError)
      end

      it "should add examples to current behaviour when calling it_should_behave_like" do
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.it("shared example") {}
        shared_behaviour.it("shared example 2") {}

        @behaviour.it("example") {}
        @behaviour.number_of_examples.should == 1
        @behaviour.it_should_behave_like("shared behaviour")
        @behaviour.number_of_examples.should == 3
      end

      it "should run shared examples" do
        shared_example_ran = false
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.it("shared example") { shared_example_ran = true }

        example_ran = false

        @behaviour.it_should_behave_like("shared behaviour")
        @behaviour.it("example") {example_ran = true}
        @behaviour.run(@formatter)
        example_ran.should be_true
        shared_example_ran.should be_true
      end

      it "should run setup and teardown from shared behaviour" do
        shared_setup_ran = false
        shared_teardown_ran = false
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.before { shared_setup_ran = true }
        shared_behaviour.after { shared_teardown_ran = true }
        shared_behaviour.it("shared example") { shared_example_ran = true }

        example_ran = false

        @behaviour.it_should_behave_like("shared behaviour")
        @behaviour.it("example") {example_ran = true}
        @behaviour.run(@formatter)
        example_ran.should be_true
        shared_setup_ran.should be_true
        shared_teardown_ran.should be_true
      end

      it "should run before(:all) and after(:all) only once from shared behaviour" do
        shared_before_all_run_count = 0
        shared_after_all_run_count = 0
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.before(:all) { shared_before_all_run_count += 1}
        shared_behaviour.after(:all) { shared_after_all_run_count += 1}
        shared_behaviour.it("shared example") { shared_example_ran = true }

        example_ran = false

        @behaviour.it_should_behave_like("shared behaviour")
        @behaviour.it("example") {example_ran = true}
        @behaviour.run(@formatter)
        example_ran.should be_true
        shared_before_all_run_count.should == 1
        shared_after_all_run_count.should == 1
      end

      it "should include modules, included into shared behaviour, into current behaviour" do
        @formatter.should_receive(:add_behaviour).with :any_args
        @formatter.should_receive(:example_finished).twice.with :any_args

        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.it("shared example") { shared_example_ran = true }

        mod1_method_called = false
        mod1 = Module.new do
          define_method :mod1_method do
            mod1_method_called = true
          end
        end

        mod2_method_called = false
        mod2 = Module.new do
          define_method :mod2_method do
            mod2_method_called = true
          end
        end

        shared_behaviour.include mod2

        @behaviour.it_should_behave_like("shared behaviour")
        @behaviour.include mod1

        @behaviour.it("test") do
          mod1_method
          mod2_method
        end
        @behaviour.run(@formatter)
        mod1_method_called.should be_true
        mod2_method_called.should be_true
      end

      it "should error if told to inherit from a class" do
        shared_behaviour = make_shared_behaviour("shared behaviour", :shared => true) {}
        shared_behaviour.it("shared example") { shared_example_ran = true }
        lambda { shared_behaviour.inherit Object }.should raise_error(ArgumentError)
      end
      
      it "should raise when named shared behaviour can not be found" do
        begin
          @behaviour.it_should_behave_like("non-existent shared behaviour")
          violated
        rescue => e
          e.message.should == "Shared Behaviour 'non-existent shared behaviour' can not be found"
        end
      end
    end
  end
end
