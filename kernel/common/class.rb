# -*- encoding: us-ascii -*-

class Class

  ##
  # Specialized initialize_copy because Class needs additional protection
  def initialize_copy(other)
    # Such a weird check.
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
