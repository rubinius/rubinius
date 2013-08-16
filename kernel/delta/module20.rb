# -*- encoding: us-ascii -*-

class Module

  def origin= origin
    @origin = origin
  end

  def prepended(mod); end
  private :prepended

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
  private :prepend

  def prepend_features(klass)
    unless klass.kind_of? Module
      raise TypeError, "invalid argument class #{klass.class}, expected Module"
    end

    if kind_of? Class
      raise TypeError, "invalid receiver class #{__class__}, expected Module"
    end

    if klass == klass.origin
      im = Rubinius::IncludedModule.new(klass)
      im.superclass = klass.direct_superclass
      klass.superclass = im
      klass.origin = im
    end
    Rubinius::Type.include_modules_from(self, klass)
    Rubinius::Type.infect(klass, self)
    self
  end
  private :prepend_features
end
