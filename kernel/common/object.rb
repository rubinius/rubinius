class Object

  ##
  # Changes this object's class to +other_klass+.  Thar be dragons.

  def class=(other_klass)
    Ruby.primitive :object_change_class_to
  end

  def initialize
  end
  private :initialize

  def display(port=$>)
    port.write self
  end

end
