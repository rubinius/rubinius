class Compiler::Node
  class Script
    def execute(e)
      @child.execute(e)
      return true
    end
  end
  
  class Newline
    def execute(e)
      @child.execute(e)
    end
  end
  
  class True
    def execute(e)
      true
    end
  end
  
  class False
    def execute(e)
      false
    end
  end
  
  class Nil
    def execute(e)
      nil
    end
  end
  
  class Self
    def execute(e)
      e.self
    end
  end
end