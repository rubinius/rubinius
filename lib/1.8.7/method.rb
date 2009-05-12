class Method
  public :name
  public :receiver
  
  def owner
    if defined_in.class == Rubinius::IncludedModule
      defined_in.module
    else
      defined_in
    end
  end
end