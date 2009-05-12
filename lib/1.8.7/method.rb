class Method
  attr_reader :name
  public :receiver
  
  def owner
    if defined_in.class == Rubinius::IncludedModule
      defined_in.module
    else
      defined_in
    end
  end
end

class UnboundMethod
  attr_reader :name
  
  def owner
    def owner
      if defined_in.class == Rubinius::IncludedModule
        defined_in.module
      else
        defined_in
      end
    end
  end    
end