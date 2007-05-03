module Spec
  module DSL
    module BehaviourCallbacks
      def before(scope=:each, &block)
        case scope
        when :each; before_each_parts << block
        when :all;  before_all_parts << block
        end
      end

      def after(scope=:each, &block)
        case scope
        when :each; after_each_parts.unshift(block)
        when :all;  after_all_parts.unshift(block)
        end
      end

      def setup(&block)
        before(:each, &block)
      end

      def teardown(&block)
        after(:each, &block)
      end

      def before_all_parts
        @before_all_parts ||= []
      end

      def after_all_parts
        @after_all_parts ||= []
      end

      def before_each_parts
        @before_each_parts ||= []
      end

      def after_each_parts
        @after_each_parts ||= []
      end
    end
  end
end
