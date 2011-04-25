##
# Classes in Ruby are first-class objects, each is an instance of
# class Class.
#
# When a new class is created (typically using <tt>class Name; ... end</tt>),
# an object of type Class is created and assigned to a global constant (Name
# in this case). When <tt>Name.new</tt> is called to create a new object, the
# new method in Class is run by default.
#
# This can be demonstrated by overriding new in Class:
#
#   class Class
#     alias old_new new
#     def new(*args)
#       puts "Creating a new #{self.name}"
#       old_new(*args)
#     end
#   end
#
#   class Name
#   end
#
#   n = Name.new
#
# *produces:*
#
#   Creating a new Name
#
# Classes, modules, and objects are interrelated. In the diagram that follows,
# the vertical arrows represent inheritance, and the parentheses meta-classes.
# All singleton classes are instances of the class Class.
#
#                            +------------------+
#                            |                  |
#              Object---->(Object)              |
#               ^  ^        ^  ^                |
#               |  |        |  |                |
#               |  |  +-----+  +---------+      |
#               |  |  |                  |      |
#               |  +-----------+         |      |
#               |     |        |         |      |
#        +------+     |     Module--->(Module)  |
#        |            |        ^         ^      |
#   OtherClass-->(OtherClass)  |         |      |
#                              |         |      |
#                            Class---->(Class)  |
#                              ^                |
#                              |                |
#                              +----------------+

class Class

  def initialize(sclass=Object, name=nil, under=nil)
    raise TypeError, "already initialized class" if @instance_type

    set_superclass sclass

    super()

    # Things (rails) depend on the fact that a normal class is in the constant
    # table and have a name BEFORE inherited is run.
    set_name_if_necessary name, under if name and under
    under.const_set name, self if under

    if sclass
      Rubinius.privately do
        sclass.inherited self
      end
    end
  end

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

  ##
  # Returns the Class object that this Class inherits from. Included Modules
  # are not considered for this purpose.

  def superclass
    cls = direct_superclass
    return nil unless cls

    while cls and cls.kind_of? Rubinius::IncludedModule
      cls = cls.direct_superclass
    end

    return cls
  end

  def inherited(name)
  end
  private :inherited

  def to_s
    if obj = Rubinius::Type.singleton_class_object(self)
      case obj
      when Class
        "#<Class:#{obj.__describe__}>"
      when Module
        "#<Class:#{obj}>"
      else
        "#<Class: #<#{obj.class.__name__}:#{obj.object_id}>>"
      end
    else
      @module_name ? @module_name.to_s : super
    end
  end

  alias_method :inspect, :to_s
  alias_method :__describe__, :to_s

end
