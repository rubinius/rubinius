# -*- encoding: us-ascii -*-

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
