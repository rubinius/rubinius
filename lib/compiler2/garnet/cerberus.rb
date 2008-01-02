class Compiler
class Node
  
  class GenerationError < RuntimeError; end
  
  def to_cerb
    raise GenerationError, "#{self.class} does not support cerb"
  end
  
  class Call
    def to_cerb
       
    end
    
    def collapse_args(e)
      unless @arguments.kind_of? Array
        raise GenerationError, "Direct arguments are the only support type"
      end
      
      args = @arguments.map { |a| a.to_cerb(e) }
      args.join(', ')
    end
    
  end
  
  class FCall
    def to_cerb(e)
      if @arguments.empty?
        "#{@method}()"
      else
        "#{@method}(#{collapse_args(e)}"
      end
    end
  end
  
end
end
