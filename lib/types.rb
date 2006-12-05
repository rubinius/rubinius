module Rubinius
  
  Types = Hash.new
  
  def self.add_type(name, mod)
    Types[name] = mod

    fel = mod::Fields
    sup_name = mod::Super rescue :Object
    if !sup_name
      tot = fel
    else
      sup = Rubinius.const_get(sup_name)
      if mod == sup
        tot = fel
      else
        mod.module_eval "include Rubinius::#{sup_name}"
      
        if sup::TotalFields == fel
          tot = fel
        else
          tot = sup::TotalFields + fel
        end
      end
    end
    
    mod.const_set("TotalFields", tot)
    mod.const_set("NumberOfFields", tot.size)
    
    tot.each_with_index do |f,i|
      mod.module_eval <<-CODE, __FILE__, __LINE__ + 1
      def #{f}
        at #{i}
      end
      
      def #{f}=(o)
        put #{i}, o
      end
      CODE
    end
    
    mod.module_eval <<-CODE, __FILE__, __LINE__ + 1
    
      def field_names
        #{tot.inspect}
      end
    
      def self.basic_class_obj(sup)
        obj = Rubinius::Class.create
        obj.instance_fields = RObject.wrap(#{tot.size})
        obj.superclass = sup
        return obj
      end
      
      def self.class_obj(sup)
        obj = basic_class_obj(sup)
        obj.create_metaclass sup.metaclass
        return obj
      end
      
      def self.allocate(extra = 0)
        cls = CPU::Global.#{name}
        raise "Cant find #{name}" unless cls
        obj = Rubinius.cpu.new_object cls, cls.instance_fields.to_int + extra
        obj.as :#{name}
        return obj
      end
    
      def self.new_blank(fields)
        Rubinius.cpu.new_object RObject.nil, fields
      end
      
      def self.create_anonymous
        obj = new_blank #{tot.size}
        obj.as :#{name}
        return obj
      end
    CODE
  end
  
  def self.cpu=(cpu)
    @@cpu = cpu
  end
  
  def self.cpu
    @@cpu
  end
end

require 'types/object'
require 'types/module'
require 'types/class'
require 'types/method_context'
require 'types/tuple'
require 'types/metaclass'
require 'types/symbol'
require 'types/bytearray'
require 'types/hash'
require 'types/string'
require 'types/array'
require 'types/method_table'
require 'types/compiled_method'
require 'types/io'
require 'types/bignum'
require 'types/regexp'