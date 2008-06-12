class Compiler

  ##
  # Unused test code

  class MacroGenerator

    def initialize(compiler)
      @compiler = compiler
    end

    def make(kind, *args)
      obj = kind.new(@compiler)
      obj.args *args
      return obj
    end

    def call(who, meth, *args)
      make Compiler::Node::Call, who, meth, args
    end

    def const(name)
      make ConstFind, name
    end

    def lit(obj)
      make Literal, obj
    end
  end
end
