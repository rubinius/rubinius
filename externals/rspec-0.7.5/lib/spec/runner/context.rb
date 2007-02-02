module Spec
  module Runner
    class ContextEvalModule < Module
    end
    class Context
      module InstanceMethods
        def initialize(name, &context_block)
          @name = name

          @context_eval_module = ContextEvalModule.new
          @context_eval_module.extend ContextEval::ModuleMethods
          @context_eval_module.include ContextEval::InstanceMethods
          before_context_eval
          @context_eval_module.class_eval(&context_block)
        end

        def before_context_eval
        end

        def inherit(klass)
          @context_eval_module.inherit klass
        end
        alias :inherit_context_eval_module_from :inherit

        def include(mod)
          @context_eval_module.include mod
        end

        def context_setup(&block)
          @context_eval_module.context_setup(&block)
        end

        def context_teardown(&block)
          @context_eval_module.context_teardown(&block)
        end

        def setup(&block)
          @context_eval_module.setup(&block)
        end

        def teardown(&block)
          @context_eval_module.teardown(&block)
        end

        def specify(spec_name, opts={}, &block)
          @context_eval_module.specify(spec_name, opts, &block)
        end

        def run(reporter, dry_run=false)
          reporter.add_context(@name)
          prepare_execution_context_class
          errors = run_context_setup(reporter, dry_run)

          specifications.each do |specification|
            specification_execution_context = execution_context(specification)
            specification_execution_context.copy_instance_variables_from(@once_only_execution_context_instance, [:@spec]) unless context_setup_block.nil?
            specification.run(reporter, setup_block, teardown_block, dry_run, specification_execution_context)
          end unless errors.length > 0
          
          run_context_teardown(reporter, dry_run)
        end

        def number_of_specs
          specifications.length
        end

        def matches? name, matcher=nil
          matcher ||= SpecMatcher.new name, @name
          specifications.each do |spec|
            return true if spec.matches_matcher? matcher
          end
          return false
        end

        def run_single_spec name
          return if @name == name
          matcher = SpecMatcher.new name, @name
          specifications.reject! do |spec|
            !spec.matches_matcher? matcher
          end
        end

        def methods
          my_methods = super
          my_methods |= @context_eval_module.methods
          my_methods
        end

        protected

        def method_missing(*args)
          @context_eval_module.method_missing(*args)
        end

        def context_setup_block
          @context_eval_module.send :context_setup_block
        end

        def context_teardown_block
          @context_eval_module.send :context_teardown_block
        end

        def specifications
          @context_eval_module.send :specifications
        end

        def setup_block
          @context_eval_module.send :setup_block
        end

        def teardown_block
          @context_eval_module.send :teardown_block
        end

        def prepare_execution_context_class
          weave_in_context_modules
          execution_context_class
        end

        def weave_in_context_modules
          mods = context_modules
          context_eval_module = @context_eval_module
          execution_context_class.class_eval do
            include context_eval_module
            mods.each do |mod|
              include mod
            end
          end
        end

        def context_modules
          @context_eval_module.send :context_modules
        end

        def execution_context_class
          @context_eval_module.send :execution_context_class
        end

        def execution_context specification
          execution_context_class.new(specification)
        end

        def run_context_setup(reporter, dry_run)
          errors = []
          unless dry_run
            begin
              @once_only_execution_context_instance = execution_context(nil)
              @once_only_execution_context_instance.instance_eval(&context_setup_block)
            rescue => e
              errors << e
              location = "context_setup"
              reporter.spec_finished(location, e, location) if reporter
            end
          end
          errors
        end
        
        def run_context_teardown(reporter, dry_run)
          unless dry_run
            begin 
              @once_only_execution_context_instance ||= execution_context(nil) 
              @once_only_execution_context_instance.instance_eval(&context_teardown_block) 
            rescue => e
              location = "context_teardown"
              reporter.spec_finished(location, e, location) if reporter
            end
          end
        end

      end
      include InstanceMethods
    end
  end
end
