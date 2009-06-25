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
# All metaclasses are instances of the class Class.
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

  protected :instance_type

  def initialize(sclass=Object)
    unless sclass.kind_of?(Class)
      raise TypeError, "superclass must be a Class (#{sclass.class} given)"
    end

    set_superclass sclass

    mc = self.metaclass
    mc.set_superclass sclass.metaclass

    super()

    # add class to sclass's subclass list, for ObjectSpace.each_object(Class)
    # NOTE: This is non-standard; Ruby does not normally track subclasses
    sclass.__send__ :add_subclass, self

    sclass.__send__ :inherited, self
  end

  ##
  # Returns the Class object that this Class inherits from. Included Modules
  # are not considered for this purpose.

  def superclass()
    cls = direct_superclass
    return nil unless cls
    while cls and cls.kind_of? Rubinius::IncludedModule
      cls = cls.direct_superclass
    end
    return cls
  end

  def add_subclass(cls)
    @subclasses ||= []
    @subclasses << cls
  end
  private :add_subclass

  def __subclasses__
    @subclasses || []
  end

  def inherited(name)
  end
  private :inherited
end

class MetaClass

  ##
  # Called to perform adding a singleton method to an
  # object when the singleton method definition occurs
  # in normal user code.
  #
  def attach_method(name, executable, scope)
    # All userland added methods start out with a serial of 1.
    executable.serial = 1

    method_table.store name, executable, :public

    executable.scope = scope
    Rubinius::VM.reset_method_cache(name)

    # Call singleton_method_added on the executable in question. There is
    # a default version in Kernel which does nothing, so we can always
    # call this.
    attached_instance.__send__ :singleton_method_added, name

    executable
  end

  def inspect
    "#<MetaClass #{attached_instance.inspect}>"
  end
end
