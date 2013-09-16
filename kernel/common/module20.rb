# -*- encoding: us-ascii -*-

class Module
  def visibility_for_aliased_method(new_name, current_name_visibility)
    special_methods = [
      :initialize,
      :initialize_copy,
      :initialize_clone,
      :initialize_dup,
      :respond_to_missing?
    ]

    if special_methods.include?(new_name)
      :private
    else
      current_name_visibility
    end
  end
  private :visibility_for_aliased_method

  def check_if_method_can_be_bound!(method)
    defined_in = method.defined_in

    unless Rubinius::Type.object_kind_of?(defined_in, Module) or
           Rubinius::Type.object_kind_of?(defined_in, self.class)
      if Rubinius::Type.singleton_class_object(defined_in)
        raise TypeError, "illegal attempt to rebind a singleton method to another object"
      end

      raise TypeError, "Must be bound to an object of kind #{defined_in}"
    end
  end
  private :check_if_method_can_be_bound!
end
