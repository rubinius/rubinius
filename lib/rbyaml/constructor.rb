require 'date'

require 'rbyaml/util'

require 'rbyaml/error'
require 'rbyaml/nodes'
require 'rbyaml/composer'

class Symbol
  def __call(obj,*args)
    obj.send(self,*args)
  end
end

class Proc
  def __call(obj,*args)
    call(obj,*args)
  end
end

class Method
  def __call(obj,*args)
    call(*args)
  end
end


module RbYAML
  class ConstructorError < YAMLError
  end
  
  class BaseConstructor
    @@yaml_constructors = {}
    @@yaml_multi_constructors = {}
    @@yaml_multi_regexps = {}
    
    def get_yaml_constructor(key)
      @@yaml_constructors[key]
    end
      
    def get_yaml_multi_constructor(key)
      @@yaml_multi_constructors[key]
    end
    
    def get_yaml_multi_regexp(key)
      @@yaml_multi_regexps[key]
    end
      
    def get_yaml_multi_regexps
      @@yaml_multi_regexps
    end
    
    def self.add_constructor(tag, constructor)
      @@yaml_constructors[tag] = constructor
    end

    def self.add_multi_constructor(tag_prefix, multi_constructor)
      @@yaml_multi_constructors[tag_prefix] = multi_constructor
      @@yaml_multi_regexps[tag_prefix] = Regexp.new("^"+Regexp.escape(tag_prefix))
    end

    def initialize(composer)
      @composer = composer
      @constructed_objects = {}
      @recursive_objects = {}
    end

    def check_data
      # If there are more documents available?
      @composer.check_node
    end

    def get_data
      # Construct and return the next document.
      construct_document(@composer.get_node) if @composer.check_node
    end
    
    def each_document
      # Iterator protocol.
      while @composer.check_node
        yield construct_document(@composer.get_node)
      end
    end 

    def construct_document(node)
      data = construct_object(node)
      @constructed_objects = {}
      @recursive_objects = {}
      data
    end

    class RecursiveProxy
      attr_writer :value
      def method_missing(*args)
        @value.send(*args)
      end
      def class
        @value.class
      end
      def to_s
        @value.to_s
      end
    end
    
    def construct_object(node)
      return @constructed_objects[node] if @constructed_objects.include?(node)
      @constructed_objects[node] = RecursiveProxy.new
      constructor = get_yaml_constructor(node.tag)
      if !constructor
        ruby_cls = RbYAML::tagged_classes[node.tag]
        if ruby_cls && (ruby_cls.method_defined?(:yaml_initialize) || ruby_cls.respond_to?(:yaml_new))
          constructor = lambda { |obj,node| send(:construct_ruby_object,ruby_cls,node) }
        else
          through = true
          for tag_prefix,reg in get_yaml_multi_regexps
            if reg =~ node.tag
              tag_suffix = node.tag[tag_prefix.length..-1]
              constructor = lambda { |obj, node| get_yaml_multi_constructor(tag_prefix).__call(self,tag_suffix, node) }
              through = false
              break
            end
          end
          if through
            ctor = get_yaml_multi_constructor(nil)
            if ctor
              constructor = lambda { |obj, node| ctor.__call(self,node.tag,node) }
            else
              ctor = get_yaml_constructor(nil)
              if ctor
                constructor = lambda { |obj, node| ctor.__call(self,node)}
              else
                constructor = lambda { |obj, node| construct_primitive(node) }
              end
            end
          end
        end
      end
      data = constructor.__call(self,node)
      @constructed_objects[node].value = data
      @constructed_objects[node] = data
      data
    end

    def construct_primitive(node)
      if node.__is_scalar
        str(node)
      elsif node.__is_sequence
        seq(node)
      elsif node.__is_mapping
        map(node)
      else
        puts node.tag
      end
    end
    
    def str(node)
      if !node.__is_scalar
        if node.__is_mapping
          for key_node in node.value.keys
            if key_node.tag == "tag:yaml.org,2002:value"
              return str(node.value[key_node])
            end
          end
        end
        raise ConstructorError.new(nil,"expected a scalar node, but found #{node.tid}")
      end
      node.value
    end

    def construct_private_type(node)
      PrivateType.new(node.tag,node.value)
    end

    def seq(node)
      raise ConstructorError.new(nil,"expected a sequence node, but found #{node.tid}") if !node.__is_sequence
      node.value.map {|child| construct_object(child) }
    end

    def map(node)
      raise ConstructorError.new(nil,"expected a mapping node, but found #{node.tid}") if !node.__is_mapping
      mapping = {}
      merge = nil
      for key_node,value_node in node.value
        if key_node.tag == "tag:yaml.org,2002:merge"
          raise ConstructorError.new("while constructing a mapping","found duplicate merge key") if !merge.nil?
          if value_node.__is_mapping
            merge = [map(value_node)]
          elsif value_node.__is_sequence
            merge = []
            for subnode in value_node.value
              if !subnode.__is_mapping
                raise ConstructorError.new("while constructing a mapping","expected a mapping for merging, but found #{subnode.tid}")
              end
              merge.unshift(map(subnode))
            end
          else
            raise ConstructorError.new("while constructing a mapping","expected a mapping or list of mappings for merging, but found #{value_node.tid}")
          end
        elsif key_node.tag == "tag:yaml.org,2002:value"
          raise ConstructorError.new("while construction a mapping","found duplicate value key") if mapping.include?("=")
          value = construct_object(value_node)
          mapping["="] = value
        else
          key = construct_object(key_node)
          value = construct_object(value_node)
          mapping[key] = value
        end
      end
      if !merge.nil?
        merge << mapping
        mapping = { }
        for submapping in merge
          mapping.merge!(submapping)
        end
      end
      mapping
    end

    def construct_pairs(node)
      raise ConstructorError.new(nil,"expected a mapping node, but found #{node.tid}") if !node.__is_mapping
      node.value.collect {|key_node,value_node| [construct_object(key_node), construct_object(value_node)] }
    end
  end

  class SafeConstructor < BaseConstructor
    @@yaml_safe_constructors = {}
    @@yaml_safe_multi_constructors = {}
    @@yaml_safe_multi_regexps = {}
    
    def get_yaml_constructor(key)
      @@yaml_safe_constructors[key] || super(key)
    end
      
    def get_yaml_multi_constructor(key)
      @@yaml_safe_multi_constructors[key] || super(key)
    end
    
    def get_yaml_multi_regexp(key)
      @@yaml_safe_multi_regexps[key] || super(key)
    end
      
    def get_yaml_multi_regexps
      super.update(@@yaml_safe_multi_regexps)
    end
    
    def self.add_constructor(tag, constructor)
      @@yaml_safe_constructors[tag] = constructor
    end

    def self.add_multi_constructor(tag_prefix, multi_constructor)
      @@yaml_safe_multi_constructors[tag_prefix] = multi_constructor
      @@yaml_safe_multi_regexps[tag_prefix] = Regexp.new("^"+Regexp.escape(tag_prefix))
    end

    def construct_yaml_null(node)
      str(node)
      nil
    end
    
    BOOL_VALUES = {
#      "y" =>       true,
#      "n" =>       false,
      "yes" =>     true,
      "no" =>      false,
      "true" =>    true,
      "false" =>   false,
      "on" =>      true,
      "off" =>     false
    }

    def construct_yaml_bool(node)
      value = str(node)
      BOOL_VALUES[value.downcase]
    end
    
    def construct_yaml_int(node)
      value = str(node).to_s
      value = value.gsub(/_/, '')
      sign = +1
      first = value[0]
      if first == ?-
        sign = -1
        value.slice!(0)
      elsif first == ?+
        value.slice!(0)
      end
      base = 10
      if value == "0"
        return 0
      elsif value[0..1] == "0b"
        value.slice!(0..1)
        base = 2
      elsif value[0..1] == "0x"
        value.slice!(0..1)
        base = 16
      elsif value[0] == ?0
        value.slice!(0)
        base = 8
      elsif value.include?(?:)
        digits = (value.split(/:/).map {|val| val.to_i}).reverse
        base = 1
        value = 0
        for digit in digits
          value += digit*base
          base *= 60
        end
        return sign*value
      else
        return sign*value.to_i
      end
      return sign*value.to_i(base)
    end

    INF_VALUE = +1.0/0.0
    NAN_VALUE = 0.0/0.0
    
    def construct_yaml_float(node)
      value = str(node).to_s
      value = value.gsub(/_/, '')
      sign = +1
      first = value[0]
      if first == ?-
        sign = -1
        value.slice!(0)
      elsif first == ?+
        value.slice!(0)
      end
      if value.downcase == ".inf"
        return sign*INF_VALUE
      elsif value.downcase == ".nan"
        return NAN_VALUE
      elsif value.include?(?:)
        digits = (value.split(/:/).map {|val| val.to_f}).reverse
        base = 1
        value = 0.0
        for digit in digits
          value += digit*base
          base *= 60
        end
        return sign*value
      else
        return value.to_f
      end
    end

    def construct_yaml_binary(node)
      value = str(node)
      value.split(/[\n\x85]|(?:\r[^\n])/).unpack 'm*'
    end

    TIMESTAMP_REGEXP = /^([0-9][0-9][0-9][0-9])-([0-9][0-9]?)-([0-9][0-9]?)(?:(?:[Tt]|[ \t]+)([0-9][0-9]?):([0-9][0-9]):([0-9][0-9])(?:\.([0-9]*))?(?:[ \t]*(?:Z|([-+][0-9][0-9]?)(?::([0-9][0-9])?)?))?)?$/

    YMD_REGEXP = /^([0-9][0-9][0-9][0-9])-([0-9][0-9]?)-([0-9][0-9]?)$/
    
    def construct_yaml_timestamp(node)
      if (match = YMD_REGEXP.match(node.value))
        values = match.captures.map {|val| val.to_i}
        return Date.new(values[0],values[1],values[2])
      end
      unless (match = TIMESTAMP_REGEXP.match(node.value))
        return construct_private_type(node)
      end
      values = match.captures.map {|val| val.to_i}
      fraction = values[6]
      if fraction != 0
        fraction *= 10 while 10*fraction < 1000
        values[6] = fraction
      end
      stamp = Time.gm(values[0],values[1],values[2],values[3],values[4],values[5],values[6])
      
      diff = values[7] * 3600 + values[8] * 60
      return stamp-diff
    end
    
    def construct_yaml_omap(node)
      # Note: we do not check for duplicate keys, because its too
      # CPU-expensive.
      raise ConstructorError.new("while constructing an ordered map","expected a sequence, but found #{node.tid}") if !node.__is_sequence
      omap = []
      for subnode in node.value
        raise ConstructorError.new("while constructing an ordered map", 
                                   "expected a mapping of length 1, but found #{subnode.tid}") if !subnode.__is_mapping
        raise ConstructorError.new("while constructing an ordered map",
                                   "expected a single mapping item, but found #{subnode.value.length} items") if subnode.value.length != 1
        key_node = subnode.value.keys[0]
        key = construct_object(key_node)
        value = construct_object(subnode.value[key_node])
        omap << [key, value]
      end
      omap
    end

    def construct_yaml_pairs(node)
      construct_yaml_omap(node)
    end

    def construct_yaml_set(node)
      Set.new(map(node).keys)
    end
    
    def construct_yaml_str(node)
      val = str(node).to_s
      val.empty? ? nil : val
    end

    def construct_yaml_seq(node)
      seq(node)
    end
    
    def construct_yaml_map(node)
      map(node)
    end

    def construct_yaml_object(node, cls)
      mapping = map(node)
      data = cls.new
      mapping.each {|key,val| data.instance_variable_set("@#{key}",val)}
      data
    end

    def construct_undefined(node)
      raise ConstructorError.new(nil,"could not determine a constructor for the tag #{node.tag}")
    end
  end
  
  SafeConstructor.add_constructor('tag:yaml.org,2002:null',:construct_yaml_null)
  SafeConstructor.add_constructor('tag:yaml.org,2002:bool',:construct_yaml_bool)
  SafeConstructor.add_constructor('tag:yaml.org,2002:int',:construct_yaml_int)
  SafeConstructor.add_constructor('tag:yaml.org,2002:float',:construct_yaml_float)
  SafeConstructor.add_constructor('tag:yaml.org,2002:binary',:construct_yaml_binary)
  SafeConstructor.add_constructor('tag:yaml.org,2002:timestamp',:construct_yaml_timestamp)
  SafeConstructor.add_constructor('tag:yaml.org,2002:omap',:construct_yaml_omap)
  SafeConstructor.add_constructor('tag:yaml.org,2002:pairs',:construct_yaml_pairs)
  SafeConstructor.add_constructor('tag:yaml.org,2002:set',:construct_yaml_set)
  SafeConstructor.add_constructor('tag:yaml.org,2002:str',:construct_yaml_str)
  SafeConstructor.add_constructor('tag:yaml.org,2002:seq',:construct_yaml_seq)
  SafeConstructor.add_constructor('tag:yaml.org,2002:map',:construct_yaml_map)
  SafeConstructor.add_constructor(nil,:construct_private_type)

  class Constructor < SafeConstructor
    @@yaml_main_constructors = {}
    @@yaml_main_multi_constructors = {}
    @@yaml_main_multi_regexps = {}
    
    def get_yaml_constructor(key)
      @@yaml_main_constructors[key] || super(key)
    end
      
    def get_yaml_multi_constructor(key)
      @@yaml_main_multi_constructors[key] || super(key)
    end
    
    def get_yaml_multi_regexp(key)
      @@yaml_main_multi_regexps[key] || super(key)
    end
      
    def get_yaml_multi_regexps
      super.update(@@yaml_main_multi_regexps)
    end
    
    def self.add_constructor(tag, constructor)
      @@yaml_main_constructors[tag] = constructor
    end

    def self.add_multi_constructor(tag_prefix, multi_constructor)
      @@yaml_main_multi_constructors[tag_prefix] = multi_constructor
      @@yaml_main_multi_regexps[tag_prefix] = Regexp.new("^"+Regexp.escape(tag_prefix))
    end

    def construct_ruby(tag,node)
      obj_class = Object
      tag.split( "::" ).each { |c| obj_class = obj_class.const_get( c ) } if tag
      o = obj_class.allocate
      mapping = map(node)
      mapping.each {|key,val| o.instance_variable_set("@#{key}",val)}
      o
    end
  end

  Constructor.add_multi_constructor("!ruby/object:",:construct_ruby)
end
