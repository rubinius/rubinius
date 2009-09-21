module Rubinius
  module AST
    module Transforms
      def self.register(category, name, klass)
        transform_map[name] = klass
        send(category) << klass
      end

      def self.transform_map
        @transform_map ||= { }
      end

      def self.[](name)
        transform_map[name]
      end

      def self.default
        @default ||= []
      end

      def self.kernel
        @kernel ||= []
      end
    end

    class PrimitiveSend < SendWithArguments
      transform :default, :primitive

      def self.match?(receiver, name, arguments)
        receiver.kind_of? ConstFind and
          receiver.name == :Ruby and
          name == :primitive
      end

      def bytecode(g)
        g.send_primitive @name
      end
    end
  end
end
