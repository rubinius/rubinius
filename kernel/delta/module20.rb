# -*- encoding: us-ascii -*-

class Module

  def origin= origin
    @origin = origin
  end

  def prepended(mod); end

  def prepend(*modules)
    modules.reverse_each do |mod|
      if !mod.kind_of?(Module) or mod.kind_of?(Class)
        raise TypeError, "wrong argument type #{mod.class} (expected Module)"
      end

      Rubinius.privately do
        mod.prepend_features self
      end

      Rubinius.privately do
        mod.prepended self
      end
    end
    self
  end

  def prepend_features(klass)
    unless klass.kind_of? Module
      raise TypeError, "invalid argument class #{klass.class}, expected Module"
    end

    insert_before = klass
    mod = self.origin
    changed = false
    constants_changed = false

    while mod

      # Check for a cyclic prepend
      if mod == klass
        raise ArgumentError, "cyclic include detected"
      end

      add = true
      k = klass.origin

      while k
        if k.kind_of? Rubinius::IncludedModule
          if k == mod
            add = false
            break
          end
        end

        k = k.direct_superclass
      end

      if add
        if mod.kind_of? Rubinius::IncludedModule
          original_mod = mod.module
        else
          original_mod = mod
        end

        Rubinius::IncludedModule.new(original_mod).attach_before klass

        changed = true
      end

      mod = mod.direct_superclass
    end

    if changed
      method_table.each do |meth, obj, vis|
        Rubinius::VM.reset_method_cache klass, meth
      end
    end

    return self
  end
end
