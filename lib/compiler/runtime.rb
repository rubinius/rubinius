# -*- encoding: us-ascii -*-

module Rubinius
  module Compiler::Runtime
    def self.matches_when(array, receiver)
      array.each { |x| return true if x === receiver }
      false
    end

    def self.unwrap_block_arg(arg)
      if arg.size == 1
        elem = arg.at(0)
        return elem if elem.kind_of?(Array)
      end

      arg
    end

    def self.find_constant_for_op_asign_or(name, scope)
      name = Rubinius::Type.coerce_to_constant_name name

      current, constant = scope.module, undefined

      while current
        if entry = current.constant_table.lookup(name)
          constant = entry.constant
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end

        current = current.direct_superclass
      end

      if instance_of?(Module)
        if entry = Object.constant_table.lookup(name)
          constant = entry.constant
          constant = constant.call if constant.kind_of?(Autoload)
          return constant
        end
      end

      nil
    end

    def self.get_flip_flop(scope, index)
      scope.flip_flops ||= {}
      scope.flip_flops[index]
    end

    def self.set_flip_flop(scope, index, value)
      scope.flip_flops ||= {}
      scope.flip_flops[index] = value
    end

    def self.rbx_marshal_constant
      name
    end

    def self.get_encoding(name)
      if defined?(Encoding)
        Encoding.find name
      else
        name
      end
    end

    def self.pre_exe
      yield
    end
  end
end

