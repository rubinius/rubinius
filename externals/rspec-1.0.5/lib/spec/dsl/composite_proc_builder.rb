module Spec
  module DSL
    class CompositeProcBuilder < Array
      def initialize(callbacks=[])
        push(*callbacks)
      end

      def proc(&error_handler)
        parts = self
        Proc.new do
          parts.collect do |part|
            begin
              if part.is_a?(UnboundMethod)
                part.bind(self).call
              else
                instance_eval(&part)
              end
            rescue Exception => e
              raise e unless error_handler
              error_handler.call(e)
              e
            end
          end
        end
      end
    end
  end
end
