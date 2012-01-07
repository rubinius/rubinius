# -*- encoding: us-ascii -*-

# MRI has an Exception class named "fatal" that is raised
# by the rb_fatal function. The class is not accessible from
# ruby because the name is begins with a lower-case letter.
# Also, the exception cannot be rescued.
#
# To support rb_fatal in the C-API, Rubinius provides the
# following FatalError class. If it clashes with code in
# the wild, we can rename it.

class FatalError < Exception
end

class Exception
  def capture_backtrace!(offset=1)
    @locations = Rubinius::VM.backtrace offset
  end
end
