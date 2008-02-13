class TypesGenerator

  def self.generate
    # Type maps that maps different C types to the C type representations we use
    type_map = {
      "char" => :char,
      "signed char" => :char,
      "__signed char" => :char,
      "unsigned char" => :uchar,
      "short" => :short,
      "signed short" => :short,
      "signed short int" => :short,
      "unsigned short" => :ushort,
      "unsigned short int" => :ushort,
      "int" => :int,
      "signed int" => :int,
      "unsigned int" => :uint,
      "long" => :long,
      "long int" => :long,
      "signed long" => :long,
      "signed long int" => :long,
      "unsigned long" => :ulong,
      "unsigned long int" => :ulong,
      "long unsigned int" => :ulong,
      "long long" => :long_long,
      "long long int" => :long_long,
      "signed long long" => :long_long,
      "signed long long int" => :long_long,
      "unsigned long long" => :ulong_long,
      "unsigned long long int" => :ulong_long,
      "char *" => :string,
      "void *" => :pointer
    }
    
    typedefs = `echo "#include <sys/types.h>" | cpp | grep typedef `
    code = ""
    typedefs.each do |type|
      # Ignore unions or structs
      next if type =~ /union|struct/
      # We don't handle typdefs like typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__))); 
      # that occur on linux
      next if type =~ /\(.*\)/
  
      # strip off the starting typedef and ending ;
      type.gsub!(/^(.*typedef\s*)/, "")
      type.gsub!(/\s*;\s*$/,"")
  
      parts = type.split(/\s+/)
      final_type = parts.pop
      def_type   = parts.join(" ")
  
      if type = type_map[def_type]
        code << "rbx.platform.typedef.#{final_type} = #{type}\n"
        type_map[final_type] = type_map[def_type]
      else
        # Fallback to an ordinary pointer if we don't know the type
        if def_type =~ /\*/
          code << "rbx.platform.typedef.#{final_type} = pointer\n"
          type_map[final_type] = :pointer
        end
      end
    end
    code
  end
end

