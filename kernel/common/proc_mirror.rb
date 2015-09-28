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

      def curry(executable, args, arity)
        args.freeze

        name = executable.lambda? ? :lambda : :proc

        Proc.__send__(name) do |*a, &b|
          all_args = args + a
          if all_args.size < arity
            curry executable, all_args, arity
          else
            executable[*all_args]
          end
        end
      end
    end
  end
end
