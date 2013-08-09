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
end
