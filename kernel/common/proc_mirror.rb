module Rubinius
  class Mirror
    class Proc < Mirror
      def self.from_block(klass, env)
        begin
          return Rubinius.invoke_primitive :proc_from_env, env, klass
        rescue Rubinius::Internal => exc
          if Type.object_kind_of? env, BlockEnvironment
            msg = "unable to create Proc from BlockEnvironment"
            raise PrimitiveFailure, msg, exc
          end
        end

        begin
          env.to_proc
        rescue Exception
          raise ArgumentError, "Unable to convert #{env.inspect} to a Proc"
        end
      end
    end
  end
end
