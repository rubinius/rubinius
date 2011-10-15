module Rubinius
  module Compiler::Runtime
    def unwrap_block_arg(arg)
      if arg.size == 1
        elem = arg.at(0)
        return elem if elem.kind_of?(Array)
      end

      arg
    end

    module_function :unwrap_block_arg

    def find_constant_for_op_asign_or(name, scope)
      name = normalize_const_name(name)

      current, constant = scope.module, undefined

      while current
        constant = current.constant_table.fetch name, undefined
        unless constant.equal?(undefined)
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end

        current = current.direct_superclass
      end

      if instance_of?(Module)
        constant = Object.constant_table.fetch name, undefined
        unless constant.equal?(undefined)
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end
      end

      nil
    end

    module_function :find_constant_for_op_asign_or

    def get_flip_flop(scope, index)
      scope.flip_flops ||= {}
      scope.flip_flops[index]
    end

    module_function :get_flip_flop

    def set_flip_flop(scope, index, value)
      scope.flip_flops ||= {}
      scope.flip_flops[index] = value
    end

    module_function :set_flip_flop

    def self.rbx_marshal_constant
      name
    end
  end
end

