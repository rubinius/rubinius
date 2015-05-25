module Rubinius
  class Mirror
    class Proc < Mirror
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
