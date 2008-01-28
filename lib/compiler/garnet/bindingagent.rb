class Compiler::Node
  
  def to_ba
    raise Error, "#{self.class} is not supported in binding expressions"
  end
    
  class Call
    
    def collapse_args(scope)
      unless @arguments.kind_of? Array
        raise Error, "Direct arguments are the only support type"
      end
      
      args = @arguments.map { |a| a.to_ba(scope) }
      args.join(', ')
    end
    
    Aliases = {
      :fixnum? => "FIXNUM_P",
      :bignum? => "BIGNUM_P",
      :ref?    => "REFERENCE",
      :to_int  => "N2I",
      :put     => "SET_FIELD",
      :at      => "NTH_FIELD"
    }
    
    Operators = [:==, :>, :<, :>=, :<=]
    
    def to_ba(scope)
      func = Aliases[@method]
      is_op = Operators.include? @method
      if func.nil? and !is_op
        raise Error, "'#{@method}' is an unknown operation"
      end
      
      if @arguments.empty?
        "#{func}(#{@object.to_ba(scope)})"
      elsif @arguments.size == 1 and is_op
        "#{@object.to_ba} #{@method} #{collapse_args(scope)}"
      else
        "#{func}(#{@object.to_ba(scope)}, #{collapse_args(scope)}"
      end
    end
  end
  
  class FCall
    
    Aliases = {
      :guard => "GUARD",
      :push => "stack_push",
      :pop =>  "stack_pop"
    }
    
    def to_ba(scope)
      unless func = Aliases[@method]
        func = @method
      end
      "#{func}(#{collapse_args(scope)})"
    end
  end
  
  class If
    def to_ba(scope)
      str = "if(#{@condition.to_ba(scope)}) {\n#{@then.to_ba(scope)}; \n}"
      if @else
        str << " else { \n#{@else.to_ba(scope)}; }"
      end
    end
  end
  
  class Block
    def to_ba(scope)
      str = ""
      @body.each { |x| str << "#{x.to_ba(scope)};\n" }
      return str
    end
  end
  
end
