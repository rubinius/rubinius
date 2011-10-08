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

    def self.rbx_marshal_constant
      name
    end
  end
end

