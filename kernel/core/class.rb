# depends on: object.rb

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

  protected :object_type
  protected :has_ivars
  protected :needs_cleanup

  def initialize(sclass=Object)
    unless sclass.kind_of?(Class)
      raise TypeError, "superclass must be a Class (#{sclass.class} given)"
    end

    @instance_fields = sclass.instance_fields
    @has_ivar = sclass.has_ivars
    @needs_cleanup = sclass.needs_cleanup
    @object_type = sclass.object_type
    @superclass = sclass

    super()

    mc = self.metaclass
    mc.set_superclass sclass.metaclass

    block = block_given?
    class_eval(&block) if block
    # add class to sclass's subclass list, for ObjectSpace.each_object(Class)
    # NOTE: This is non-standard; Ruby does not normally track subclasses
    sclass.__send__ :inherited, self
  end

  def opened_class_cv(cls)
    cls = Object unless cls
    cls.add_subclass(self)

    cls.send :inherited, self

    self # FIXME: hook calls should preserve the stack
  end

  ##
  #--
  # NOTE: This method is not standard Ruby; JRuby implements it, but not
  # public
  #++

  def add_subclass_cv(cls)
    @subclasses ||= []
    @subclasses << cls
  end

  ##
  #--
  # NOTE: This method is not standard Ruby; JRuby implements it, but not
  # public
  #++

  def subclasses_cv(descend = false)
    if descend
      subclasses_descend()
    elsif @subclasses
      @subclasses.dup
    else
      []
    end
  end

  def subclasses_descend(all = [])
    return unless @subclasses
    @subclasses.each {|cls| all << cls; cls.subclasses_descend(all)}
    all
  end

  ##
  # Returns the Class object that this Class inherits from. Included Modules
  # are not considered for this purpose.

  def superclass()
    cls = direct_superclass
    return nil unless cls
    while cls and cls.kind_of? IncludedModule
      cls = cls.direct_superclass
    end
    return cls
  end

  def inherited(name)
  end

  def self.after_loaded
    alias_method :opened_class, :opened_class_cv
    alias_method :add_subclass, :add_subclass_cv
    alias_method :__subclasses__, :subclasses_cv
    alias_method :dup, :clone
  end
end

class MetaClass

  ivar_as_index :superclass => 6

  ##
  # Called when <tt>def obj.name</tt> syntax is used in userland

  def attach_method(name, object)
    # All userland added methods start out with a serial of 1.
    object.serial = 1

    cur = method_table[name]
    if cur and cur.kind_of? Tuple
      # Override the slot which points to the method, so that we
      # retain visibility.
      cur[1] = object
    else
      method_table[name] = Tuple[:public, object]
    end

    object.inherit_scope MethodContext.current.sender.method
    Rubinius::VM.reset_method_cache(name)

    # Call singleton_method_added on the object in question. There is
    # a default version in Kernel which does nothing, so we can always
    # call this.
    attached_instance.__send__ :singleton_method_added, name

    return object
  end

  def set_superclass(obj)
    @superclass = obj
  end

  def inspect
    "#<MetaClass #{attached_instance.inspect}>"
  end  
end
