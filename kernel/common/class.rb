# -*- encoding: us-ascii -*-

class Class

  ##
  # Specialized initialize_copy because Class needs additional protection
  def initialize_copy(other)
    # The code in super (Module#initialize_copy) will duplicate
    # the method table. Therefore do this check here to see whether
    # we've already initialized this class before continuing.
    unless @method_table == other.method_table
      raise TypeError, "already initialized class"
    end

    @module_name = nil
    super
  end

  def inherited(name)
  end
  private :inherited

end
