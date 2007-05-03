module Spec
  module DSL
    class Configuration
      
      def mock_with(mock_framework)
        @mock_framework = case mock_framework
        when Symbol
          mock_framework_path(mock_framework.to_s)
        else
          mock_framework
        end
      end
      
      def mock_framework
        @mock_framework ||= mock_framework_path("rspec")
      end
      
      def include(mod)
        included_modules << mod
      end
      
      def included_modules
        @included_modules ||= []
      end
      
      def predicate_matchers
        @predicate_matchers ||= {}
      end
      
    private
    
      def mock_framework_path(framework_name)
        File.expand_path(File.join(File.dirname(__FILE__), "..", "..", "..", "plugins", "mock_frameworks", framework_name))
      end
      
    end
  end
end