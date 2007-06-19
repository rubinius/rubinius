module Spec
  module DSL
    class EvalModule < Module; end
    class Behaviour
      extend BehaviourCallbacks

      class << self
        def add_shared_behaviour(behaviour)
          return if behaviour.equal?(found_behaviour = find_shared_behaviour(behaviour.description))
          return if found_behaviour and behaviour.description[:spec_path] == found_behaviour.description[:spec_path]
          raise ArgumentError.new("Shared Behaviour '#{behaviour.description}' already exists") if found_behaviour
          shared_behaviours << behaviour
        end

        def find_shared_behaviour(behaviour_description)
          shared_behaviours.find { |b| b.description == behaviour_description }
        end

        def shared_behaviours
          # TODO - this needs to be global, or at least accessible from
          # from subclasses of Behaviour in a centralized place. I'm not loving
          # this as a solution, but it works for now.
          $shared_behaviours ||= []
        end
      end

      def initialize(*args, &behaviour_block)
        init_description(*args)
        init_eval_module
        before_eval
        eval_behaviour(&behaviour_block)
      end
      
    private
    
      def init_description(*args)
        unless self.class == Behaviour
          args << {} unless Hash === args.last
          args.last[:behaviour_class] = self.class
        end
        @description = Description.new(*args)
      end
      
      def init_eval_module
        @eval_module = EvalModule.new
        @eval_module.extend BehaviourEval::ModuleMethods
        @eval_module.include BehaviourEval::InstanceMethods
        @eval_module.include described_type if described_type.class == Module
        @eval_module.behaviour = self
        @eval_module.description = @description
      end

      def eval_behaviour(&behaviour_block)
        @eval_module.class_eval(&behaviour_block)
      end
      
    protected
    
      def before_eval
      end
      
    public

      def run(reporter, dry_run=false, reverse=false, timeout=nil)
        raise "shared behaviours should never run" if shared?
        reporter.add_behaviour(description)
        prepare_execution_context_class
        errors = run_before_all(reporter, dry_run)

        specs = reverse ? examples.reverse : examples
        example_execution_context = nil
         
        if errors.empty?
          specs.each do |example|
            example_execution_context = execution_context(example)
            example_execution_context.copy_instance_variables_from(@before_and_after_all_context_instance) unless before_all_proc(behaviour_type).nil?
            example.run(reporter, before_each_proc(behaviour_type), after_each_proc(behaviour_type), dry_run, example_execution_context, timeout)
          end
        end
        
        @before_and_after_all_context_instance.copy_instance_variables_from(example_execution_context) unless after_all_proc(behaviour_type).nil?
        run_after_all(reporter, dry_run)
      end

      def number_of_examples
        examples.length
      end

      def matches?(specified_examples)
        matcher ||= ExampleMatcher.new(description)

        examples.each do |example|
          return true if example.matches?(matcher, specified_examples)
        end
        return false
      end

      def shared?
        @description[:shared]
      end

      def retain_examples_matching!(specified_examples)
        return if specified_examples.index(description)
        matcher = ExampleMatcher.new(description)
        examples.reject! do |example|
          !example.matches?(matcher, specified_examples)
        end
      end

      def methods
        my_methods = super
        my_methods |= @eval_module.methods
        my_methods
      end

      # Includes modules in the Behaviour (the <tt>describe</tt> block).
      def include(*args)
        args << {} unless Hash === args.last
        modules, options = args_and_options(*args)
        required_behaviour_type = options[:behaviour_type]
        if required_behaviour_type.nil? || required_behaviour_type.to_sym == behaviour_type.to_sym
          @eval_module.include(*modules)
        end
      end

      def behaviour_type #:nodoc:
        @description[:behaviour_type]
      end

    protected

      # Messages that this class does not understand
      # are passed directly to the @eval_module.
      def method_missing(sym, *args, &block)
        @eval_module.send(sym, *args, &block)
      end

      def prepare_execution_context_class
        plugin_mock_framework
        weave_in_included_modules
        define_predicate_matchers #this is in behaviour_eval
        execution_context_class
      end

      def weave_in_included_modules
        mods = included_modules
        eval_module = @eval_module
        execution_context_class.class_eval do
          include eval_module
          Spec::Runner.configuration.included_modules.each do |mod|
            include mod
          end
          mods.each do |mod|
            include mod
          end
        end
      end

      def execution_context(example)
        execution_context_class.new(example)
      end

      def run_before_all(reporter, dry_run)
        errors = []
        unless dry_run
          begin
            @before_and_after_all_context_instance = execution_context(nil)
            @before_and_after_all_context_instance.instance_eval(&before_all_proc(behaviour_type))
          rescue => e
            errors << e
            location = "before(:all)"
            reporter.example_finished(location, e, location) if reporter
          end
        end
        errors
      end
      
      def run_after_all(reporter, dry_run)
        unless dry_run
          begin 
            @before_and_after_all_context_instance ||= execution_context(nil) 
            @before_and_after_all_context_instance.instance_eval(&after_all_proc(behaviour_type)) 
          rescue => e
            location = "after(:all)"
            reporter.example_finished(location, e, location) if reporter
          end
        end
      end
      
      def plugin_mock_framework
        case mock_framework = Spec::Runner.configuration.mock_framework
        when Module
          include mock_framework
        else
          require Spec::Runner.configuration.mock_framework
          include Spec::Plugins::MockFramework
        end
      end
      
      def description
        @description.to_s
      end
      
      def described_type
        @description.described_type
      end

    end
  end
end
