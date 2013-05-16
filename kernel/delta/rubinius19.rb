# -*- encoding: us-ascii -*-

module Rubinius
  def self.open_class_under(name, sup, mod)
    unless Type.object_kind_of? mod, Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constant_table
    if !tbl.has_name?(name)
      # Create the class
      sup = Object unless sup
      obj = Class.new sup, name, mod
    else
      entry = tbl.lookup(name)
      if entry.visibility == :private
        unless self == Object
          mod_name = "#{Rubinius::Type.module_name mod}::"
        end
        raise NameError, "Private constant: #{mod_name}#{name}"
      end
      obj = entry.constant
      if Type.object_kind_of? obj, Autoload
        obj = begin
          obj.call(mod, true)
        rescue NameError # Constant not found in loaded file
          nil
        end

        # nil is returned if the autoload was abort, usually because
        # the file to be required has already been loaded. In which case
        # act like the autoload wasn't there.
        unless obj
          supr = sup ? sup : Object
          obj = Class.new supr, name, mod
        end
      end

      if Type.object_kind_of? obj, Class
        if sup and obj.superclass != sup
          raise TypeError, "Superclass mismatch: #{obj.superclass} != #{sup}"
        end
      else
        raise TypeError, "#{name} is not a class"
      end
    end

    return obj
  end
end
