# -*- encoding: us-ascii -*-

module Rubinius
  module Type
    def self.object_respond_to__dump?(obj)
      object_respond_to? obj, :_dump, true
    end

    def self.object_respond_to_marshal_dump?(obj)
      object_respond_to? obj, :marshal_dump, true
    end

    def self.object_respond_to_marshal_load?(obj)
      object_respond_to? obj, :marshal_load, true
    end

    def self.bindable_method?(klass, method)
      defined_in = method.defined_in

      unless object_kind_of? defined_in, Module or
             object_kind_of? defined_in, klass
        if singleton_class_object defined_in
          raise TypeError, "illegal attempt to rebind a singleton method to another object"
        end

        raise TypeError, "Must be bound to an object of kind #{defined_in}"
      end
    end
  end
end
