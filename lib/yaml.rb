require 'sexp/simple_processor'
require 'ext/syck/rbxext'

module YAML
  
  StringTag = :"tag:yaml.org,2002:str"
  NumberTag = :"tag:yaml.org,2002:int"
  FloatTag =  :"tag:yaml.org,2002:float#fix"
  YesTag  =   :"tag:yaml.org,2002:bool#yes"
  NoTag =     :"tag:yaml.org,2002:bool#no"
  RegexpTag = :"tag:ruby.yaml.org,2002:regexp"
  ObjectPrefix = /tag:ruby.yaml.org,2002:object:(.+)/
  
  class Processor < SimpleSexpProcessor
    
    def initialize(resolver)
      super()
      @resolver = resolver
      self.auto_shift_type = true
      self.strict = false
    end
    
    def process_map(x)
      type = x.shift
      style = x.shift
      out = {}
      until x.empty?
        k = process(x.shift)
        v = process(x.shift)
        out[k] = v
      end
      
      if type
        out = @resolver.resolve_map type, out
      end
      
      return out
    end
    
    def process_sequence(x)
      type = x.shift
      style = x.shift
      out = []
      until x.empty?
        out << process(x.shift)
      end
      
      return out
    end
    
    def process_scalar(x)
      type = x.shift
      style = x.shift
      val = x.shift
      
      if type
        @resolver.resolve_scalar(type, val)
      else
        val
      end
    end
  end
  
  class Object
    def initialize(type, attrs)
      @type = type
      @attributes = attrs
    end
    
    attr_accessor :type, :attributes
  end
  
  class Scalar
    def initialize(type, val)
      @type = type
      @val = val
    end
    
    attr_accessor :type, :val
  end
  
  class Emitter
    
    def emit_scalar(obj)
      case obj
      when YAML::Scalar
        [:scalar, obj.type, nil, obj.val]
      when String
        [:scalar, StringTag, nil, obj]
      when Integer
        [:scalar, NumberTag, nil, obj.to_s]
      when Float
        [:scalar, FloatTag, nil, obj.to_s]
      when Regexp
        [:scalar, RegexpTag, nil, "/#{obj.source}/"]
      when TrueClass
        [:scalar, YesTag, nil, "true"]
      when FalseClass
        [:scalar, NoTag, nil, "false"]
      when Symbol
        [:scalar, StringTag, nil, ":#{obj}"]
      else
        raise "Unknown scalar '#{obj.inspect}'"
      end
    end
    
    def emit_object(obj)
      out = [:map, "tag:ruby.yaml.org,2002:object:#{obj.class}", nil]
      obj.instance_variables.each do |ivar|
        out << emit(ivar[1..-1])
        out << emit(obj.instance_variable_get(ivar))
      end
      return out
    end
    
    def emit(obj)
      case obj
      when YAML::Scalar, String, Numeric, TrueClass, FalseClass, Regexp, Symbol
        emit_scalar(obj)
      when Array, Tuple
        out = [:sequence, nil, nil]
        obj.each do |o|
          out << emit(o)
        end
        return out
      when Hash
        out = [:map, nil, nil]
        obj.each do |k,v|
          out << emit(k)
          out << emit(v)
        end
        return out
      else
        emit_object(obj)
      end
    end
  end
  
  class Resolver
        
    def resolve_scalar(type, val)
      case type
      when StringTag
        if val[0] == ?:
          val[1..-1].to_sym
        else
          val
        end
      when NumberTag
        val.to_i
      when FloatTag
        val.to_f
      when RegexpTag
        Regexp.new(val[1..-2])
      when YesTag
        true
      when NoTag
        false
      else
        YAML::Scalar.new(type, val)
      end
    end
    
    def extract_class(type)
      str = type.to_s
      if m = ObjectPrefix.match(str)
        name = m[1]
        begin
          cls = Object
          parts = name.split("::")
          until parts.empty?
            cls = cls.const_get(parts.shift)
            raise NameError unless cls
          end
          
          return cls
        rescue Object
          return nil
        end
      end
      
      return nil
    end
    
    def resolve_map(type, val)
      if cls = extract_class(type)
        # This mirrors syck's object allocation.
        if cls.respond_to? :yaml_new
          obj = cls.yaml_new(nil, type, val)
        else
          obj = cls.allocate
          if obj.respond_to? :yaml_initialize
            obj.yaml_initialize(type, val)
          else
            val.each do |k, v|
              obj.instance_variable_set("@#{k}", v)
            end
          end
        end
      
        return obj
      else
        return YAML::Object.new(type, val)
      end
    end
  end
    
  def self.load(thing, resolver=Resolver.new)
    if thing.kind_of? IO
      thing = thing.read
    end
    data = parse_string(thing)
    raise "Unable to parse data" unless data
    
    pro = Processor.new(resolver)
    return pro.process(data)
  end
  
  def self.load_file(name, resolver=Resolver.new)
    data = parse_file(name)
    raise "Unable to parse '#{name}'" unless data
    
    pro = Processor.new(resolver)
    return pro.process(data)
  end
  
  def self.emit(obj, emitter=Emitter.new)
    emit_yaml emitter.emit(obj)
  end
end

class Object
  def to_yaml
    YAML.emit(self) 
  end
end
