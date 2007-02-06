module PrimitiveGenerator
  def self.generate(method)
    @types = [] # clean slate before going
    code = method.call
    generate_checks + code
  end

  def self.generate_checks
    arity = @types.length - 1
    raise "Primitive MUST pop self" unless arity >= 0
    variables = %w(self t1 t2 t3)

    rval = "    ARITY(#{arity})\n"
    while (type = @types.shift)
      variable = variables.shift
      rval << "    POP(#{variable}, #{type})\n"
    end
    rval
  end

  def self.push_type(name)
    @types ||= []
    @types << name.to_s.upcase
  end

  def method_missing(name, *args, &block)
    PrimitiveGenerator.push_type(name)
  end
end

