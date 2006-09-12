require 'translation/typer'

class GenerateHeader
  def initialize(klass, ti)
    @klass = klass
    @info = ti
    @prefix = ""
  end
  
  def klass_name
    @prefix + @klass.name.to_s
  end
  
  def generate_fields(klass)
    fields = []
    sorted = klass.ivars.sort do |a,b|
      a[0].to_s <=> b[0].to_s
    end
    
    sorted.each do |name, type|
      c_type = @info.to_c_instance(type)
      fields << "  #{c_type} #{name};"
    end
    
    return fields
  end
  
  attr_accessor :prefix
  
  def generate_struct
    super_fields = []
    sup = @klass.superklass
    while sup and sup != :Object
      obj = @info.classes[sup]
      super_fields << generate_fields(obj)
      sup = obj.superklass
    end
    
    fields = []
    super_fields.reverse.each do |set|
      fields += set
    end
    
    fields += generate_fields(@klass)
    
    "struct #{klass_name} {\n#{fields.join("\n")}\n};\n"
  end
  
  def generate_functions
    out = []
    @klass.defined_methods.each do |name, meth|
      c_type = @info.to_c_instance(meth.type)
      c_name = klass_name + "_" + name.to_s
      args = meth.args.map do |a_name, a_type|
        "#{@info.to_c_instance(a_type)} #{a_name}"
      end
      if args.size == 0
        str_args = ""
      else
        str_args = ", " + args.join(", ")
      end
      out << "#{c_type} #{c_name}(struct #{klass_name} *self#{str_args});"
    end
    out.join("\n") + "\n"
  end
  
  def generate
    str = generate_struct
    str << "\n"
    str << generate_functions
  end
end