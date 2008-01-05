
module RbYAML
  class YAMLError < StandardError
    def initialize(context=nil, problem=nil, note=nil)
      super()
      @context = context
      @problem = problem
      @note = note
    end
    
    def to_s
      lines = []
      lines << @context if @context
      lines << @problem if @problem
      lines << @note if @note
      lines.join("\n")
    end
  end

  class TypeError < YAMLError
  end
end
