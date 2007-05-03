module Spec
  module DSL
    class BehaviourFactory

      class << self

        BEHAVIOUR_CLASSES = {:default => Spec::DSL::Behaviour}
        
        def add_behaviour_class(key, value)
          BEHAVIOUR_CLASSES[key] = value
        end

        def remove_behaviour_class(key)
          BEHAVIOUR_CLASSES.delete(key)
        end

        def create(*args, &block)
          return BEHAVIOUR_CLASSES[behaviour_type(*args)].new(*args, &block)
        end
        
      private
      
        def behaviour_type(*args)
          opts = Hash === args.last ? args.last : {}
          opts[:behaviour_type] ? opts[:behaviour_type] : :default
        end
        
      end
      
    end
  end
end
