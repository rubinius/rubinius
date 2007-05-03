require File.dirname(__FILE__) + '/../../spec_helper'

module Spec
  module DSL
    class FakeReporter < Spec::Runner::Reporter
      attr_reader :added_behaviour
      def add_behaviour(description)
        @added_behaviour = description
      end
    end

    describe Behaviour, "class methods" do
      before :each do
        @original_before_all_parts = Behaviour.before_all_parts.dup
        @original_after_all_parts = Behaviour.after_all_parts.dup
        @original_before_each_parts = Behaviour.before_each_parts.dup
        @original_after_each_parts = Behaviour.after_each_parts.dup

        @reporter = FakeReporter.new(mock("formatter", :null_object => true), mock("backtrace_tweaker", :null_object => true))
        @behaviour = Behaviour.new("example") {}
      end

      after :each do
        Behaviour.instance_variable_set(:@before_all_parts, @original_before_all_parts)
        Behaviour.instance_variable_set(:@after_all_parts, @original_after_all_parts)
        Behaviour.instance_variable_set(:@before_each_parts, @original_before_each_parts)
        Behaviour.instance_variable_set(:@after_each_parts, @original_after_each_parts)
      end

      it "should not run before(:all) or after(:all) on dry run" do
        before_all_ran = false
        after_all_ran = false
        Behaviour.before(:all) { before_all_ran = true }
        Behaviour.after(:all) { after_all_ran = true }
        @behaviour.it("should") {}
        @behaviour.run(@reporter, true)
        before_all_ran.should be_false
        after_all_ran.should be_false
      end

      it "should not run any example if before(:all) fails" do
        spec_ran = false
        Behaviour.before(:all) { raise "help" }
        @behaviour.it("test") {spec_ran = true}
        @behaviour.run(@reporter)
        spec_ran.should be_false
      end

      it "should run after(:all) if before(:all) fails" do
        after_all_ran = false
        Behaviour.before(:all) { raise }
        Behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should run after(:all) if before(:each) fails" do
        after_all_ran = false
        Behaviour.before(:each) { raise }
        Behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should run after(:all) if any example fails" do
        after_all_ran = false
        @behaviour.it("should") { raise "before all error" }
        Behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should supply before(:all) as description if failure in before(:all)" do
        @reporter.should_receive(:example_finished) do |name, error, location|
          name.should eql("before(:all)")
          error.message.should eql("in before(:all)")
          location.should eql("before(:all)")
        end

        Behaviour.before(:all) { raise "in before(:all)" }
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
      end

      it "should provide after(:all) as description if failure in after(:all)" do
        @reporter.should_receive(:example_finished) do |name, error, location|
          name.should eql("after(:all)")
          error.message.should eql("in after(:all)")
          location.should eql("after(:all)")
        end

        Behaviour.after(:all) { raise "in after(:all)" }
        @behaviour.run(@reporter)
      end
    end

    describe Behaviour do
      before :each do
        @original_before_all_parts = Behaviour.before_all_parts.dup
        @original_after_all_parts = Behaviour.after_all_parts.dup
        @original_before_each_parts = Behaviour.before_each_parts.dup
        @original_after_each_parts = Behaviour.after_each_parts.dup

        @reporter = FakeReporter.new(mock("formatter", :null_object => true), mock("backtrace_tweaker", :null_object => true))
        @behaviour = Behaviour.new("example") {}
      end

      after :each do
        Behaviour.instance_variable_set(:@before_all_parts, @original_before_all_parts)
        Behaviour.instance_variable_set(:@after_all_parts, @original_after_all_parts)
        Behaviour.instance_variable_set(:@before_each_parts, @original_before_each_parts)
        Behaviour.instance_variable_set(:@after_each_parts, @original_after_each_parts)
      end

      it "should send reporter add_behaviour" do
        @behaviour.run(@reporter)
        @reporter.added_behaviour.should == "example"
      end

      it "should run example on run" do
        example_ran = false
        @behaviour.it("should") {example_ran = true}
        @behaviour.run(@reporter)
        example_ran.should be_true
      end

      it "should not run example on dry run" do
        example_ran = false
        @behaviour.it("should") {example_ran = true}
        @behaviour.run(@reporter, true)
        example_ran.should be_false
      end

      it "should not run before(:all) or after(:all) on dry run" do
        before_all_ran = false
        after_all_ran = false
        @behaviour.before(:all) { before_all_ran = true }
        @behaviour.after(:all) { after_all_ran = true }
        @behaviour.it("should") {}
        @behaviour.run(@reporter, true)
        before_all_ran.should be_false
        after_all_ran.should be_false
      end

      it "should not run any example if before(:all) fails" do
        spec_ran = false
        @behaviour.before(:all) { raise "help" }
        @behaviour.it("test") {spec_ran = true}
        @behaviour.run(@reporter)
        spec_ran.should be_false
      end

      it "should run after(:all) if before(:all) fails" do
        after_all_ran = false
        @behaviour.before(:all) { raise }
        @behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should run after(:all) if before(:each) fails" do
        after_all_ran = false
        @behaviour.before(:each) { raise }
        @behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should run after(:all) if any example fails" do
        after_all_ran = false
        @behaviour.it("should") { raise "before all error" }
        @behaviour.after(:all) { after_all_ran = true }
        @behaviour.run(@reporter)
        after_all_ran.should be_true
      end

      it "should supply before(:all) as description if failure in before(:all)" do
        @reporter.should_receive(:example_finished) do |name, error, location|
          name.should eql("before(:all)")
          error.message.should eql("in before(:all)")
          location.should eql("before(:all)")
        end

        @behaviour.before(:all) { raise "in before(:all)" }
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
      end

      it "should provide after(:all) as description if failure in after(:all)" do
        @reporter.should_receive(:example_finished) do |name, error, location|
          name.should eql("after(:all)")
          error.message.should eql("in after(:all)")
          location.should eql("after(:all)")
        end

        @behaviour.after(:all) { raise "in after(:all)" }
        @behaviour.run(@reporter)
      end

      it "should run before(:all) block only once" do
        before_all_run_count_run_count = 0
        @behaviour.before(:all) {before_all_run_count_run_count += 1}
        @behaviour.it("test") {true}
        @behaviour.it("test2") {true}
        @behaviour.run(@reporter)
        before_all_run_count_run_count.should == 1
      end

      it "should run superclass setup method and before block" do
        super_class_before_ran = false
        super_class = Class.new do
          define_method :setup do
            super_class_before_ran = true
          end
        end
        @behaviour.inherit super_class
    
        before_ran = false
        @behaviour.before {before_ran = true}
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
        super_class_before_ran.should be_true
        before_ran.should be_true
      end
    
      it "should run after(:all) block only once" do
        after_all_run_count = 0
        @behaviour.after(:all) {after_all_run_count += 1}
        @behaviour.it("test") {true}
        @behaviour.it("test2") {true}
        @behaviour.run(@reporter)
        after_all_run_count.should == 1
        @reporter.rspec_verify
      end

      it "after(:all) should have access to all instance variables defined in before(:all)" do
        context_instance_value_in = "Hello there"
        context_instance_value_out = ""
        @behaviour.before(:all) { @instance_var = context_instance_value_in }
        @behaviour.after(:all) { context_instance_value_out = @instance_var }
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
        context_instance_value_in.should == context_instance_value_out
      end
    
      it "should copy instance variables from before(:all)'s execution context into spec's execution context" do
        context_instance_value_in = "Hello there"
        context_instance_value_out = ""
        @behaviour.before(:all) { @instance_var = context_instance_value_in }
        @behaviour.it("test") {context_instance_value_out = @instance_var}
        @behaviour.run(@reporter)
        context_instance_value_in.should == context_instance_value_out
      end
    
      it "before callbacks are ordered from global to local" do
        fiddle = []
        super_class = Class.new do
          define_method :setup do
            fiddle << "superclass setup"
          end
        end
        @behaviour.inherit super_class

        Behaviour.before(:all) { fiddle << "Behaviour.before(:all)" }
        @behaviour.before(:all) { fiddle << "before(:all)" }
        @behaviour.before(:each) { fiddle << "before(:each)" }
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
        fiddle.should == ['Behaviour.before(:all)', 'before(:all)', 'superclass setup', 'before(:each)']
      end

      it "after callbacks are ordered from local to global" do
        @reporter.should_receive(:add_behaviour).with :any_args
        @reporter.should_receive(:example_finished).with :any_args

        fiddle = []
        super_class = Class.new do
          define_method :teardown do
            fiddle << "superclass teardown"
          end
        end
        @behaviour.inherit super_class

        @behaviour.after(:all) { fiddle << "after(:all)" }
        Behaviour.after(:all) { fiddle << "Behaviour.after(:all)" }
        @behaviour.after(:each) { fiddle << "after(:each)" }
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
        fiddle.should == ['after(:each)', 'superclass teardown', 'after(:all)', 'Behaviour.after(:all)']
      end
    
      it "should run superclass teardown method and after block" do
        super_class_teardown_ran = false
        super_class = Class.new do
          define_method :teardown do
            super_class_teardown_ran = true
          end
        end
        @behaviour.inherit super_class
    
        teardown_ran = false
        @behaviour.after {teardown_ran = true}
        @behaviour.it("test") {true}
        @behaviour.run(@reporter)
        super_class_teardown_ran.should be_true
        teardown_ran.should be_true
        @reporter.rspec_verify
      end
    
      it "should have accessible methods from inherited superclass" do
        helper_method_ran = false
        super_class = Class.new do
          define_method :helper_method do
            helper_method_ran = true
          end
        end
        @behaviour.inherit super_class
    
        @behaviour.it("test") {helper_method}
        @behaviour.run(@reporter)
        helper_method_ran.should be_true
      end
    
      it "should have accessible class methods from inherited superclass" do
        class_method_ran = false
        super_class = Class.new
        (class << super_class; self; end).class_eval do
          define_method :class_method do
            class_method_ran = true
          end
        end
        @behaviour.inherit super_class
        @behaviour.class_method
        class_method_ran.should be_true
    
        lambda {@behaviour.foobar}.should raise_error(NoMethodError)
      end
    
      it "should include inherited class methods" do
        class_method_ran = false
        super_class = Class.new
        class << super_class
          def super_class_class_method; end
        end
        @behaviour.inherit super_class
    
        @behaviour.methods.should include("super_class_class_method")
      end
    
      it "should have accessible instance methods from included module" do
        @reporter.should_receive(:add_behaviour).with :any_args
        @reporter.should_receive(:example_finished).with :any_args
    
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
    
        @behaviour.include mod1
        @behaviour.include mod2
    
        @behaviour.it("test") do
          mod1_method
          mod2_method
        end
        @behaviour.run(@reporter)
        mod1_method_called.should be_true
        mod2_method_called.should be_true
      end

      it "should have accessible class methods from included module" do
        mod1_method_called = false
        mod1 = Module.new do
          class_methods = Module.new do
              define_method :mod1_method do
                mod1_method_called = true
              end
          end

          metaclass.class_eval do
            define_method(:included) do |receiver|
              receiver.extend class_methods
            end
          end
        end
    
        mod2_method_called = false
        mod2 = Module.new do
          class_methods = Module.new do
              define_method :mod2_method do
                mod2_method_called = true
              end
          end

          metaclass.class_eval do
            define_method(:included) do |receiver|
              receiver.extend class_methods
            end
          end
        end
    
        @behaviour.include mod1
        @behaviour.include mod2
    
        @behaviour.mod1_method
        @behaviour.mod2_method
        mod1_method_called.should be_true
        mod2_method_called.should be_true
      end

      it "should count number of specs" do
        @behaviour.it("one") {}
        @behaviour.it("two") {}
        @behaviour.it("three") {}
        @behaviour.it("four") {}
        @behaviour.number_of_examples.should == 4
      end

      it "should not match anything when there are no examples" do
        @behaviour.should_not be_matches(['context'])
      end
    
      it "should match when one of the examples match" do
        example = mock('my example')
        example.should_receive(:matches?).and_return(true)
        @behaviour.stub!(:examples).and_return([example])
        @behaviour.should be_matches(['jalla'])
      end
      
      it "should include any modules included using configuration" do
        mod = Module.new do
          class << self
            def included(mod)
              $included_module = mod
            end
          end
        end

        begin
          $included_module = nil
          Spec::Runner.configuration.include(mod)

          behaviour = Behaviour.new('example') do
          end.run(@reporter)
        
          $included_module.should_not be_nil
        ensure
          Spec::Runner.configuration.included_modules.delete(mod)
        end
      end
      
      it "should include any predicate_matchers included using configuration" do
        $included_predicate_matcher_found = false
        Spec::Runner.configuration.predicate_matchers[:does_something?] = :do_something
        Behaviour.new('example') do
          it "should respond to do_something" do
            $included_predicate_matcher_found = respond_to?(:do_something)
          end
        end.run(@reporter)
        $included_predicate_matcher_found.should be(true)
      end
      
      it "should use a mock framework set up in config" do
        mod = Module.new do
          class << self
            def included(mod)
              $included_module = mod
            end
          end
        end

        begin
          $included_module = nil
          Spec::Runner.configuration.mock_with mod

          behaviour = Behaviour.new('example') do
          end.run(@reporter)
        
          $included_module.should_not be_nil
        ensure
          Spec::Runner.configuration.mock_with :rspec
        end
      end

    end      
    
    class BehaviourSubclass < Behaviour
      public :described_type
    end
    
    describe Behaviour, " subclass" do
      it "should have access to the described_type" do
        BehaviourSubclass.new(Example){}.described_type.should == Example
      end
      
      # TODO - add an example about shared behaviours
    end
  end
end
