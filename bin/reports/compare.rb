# Matthew Elder <sseses@gmail.com>
# See what methods were missing on core ruby classes for rubinius
# Next Generation with code cleanup! (saweet)
# Now does all modules

# have to tweak MRI Class
class Class
  class << self
    def ivar_as_index(*args)
    end

    def instance_fields=(*args)
    end

    def index_reader(*args)
    end

    def define_fields(*args)
    end
  end
end

class Object
  def ivar_as_index(*args)
  end

  def instance_fields=(*args)
  end

  def index_reader(*args)
  end

  def define_fields(*args)
  end
end


# module to hold rubinius core namespace
module Rubinius
  BASE = %w(__ ivar_as_index instance_fields index_reader define_fields to_s
            class methods instance_methods superclass instance_of?)
  BASE_EXP = Regexp.new('^' + BASE.join('|^'))

  module Ruby
    def self.primitive(*args)
    end

    def self.asm(*args)
    end
  end

  class Object < ::Object
    instance_methods.each {|m| undef_method m unless m =~ BASE_EXP }

    def kind_of?(klass)
      [Rubinius::Object, Rubinius::Module, Rubinius::Class].include?(klass)
      true
    end

    class << self
      instance_methods.each {|m| undef_method m unless m =~ BASE_EXP }
      def kind_of?(klass)
        [Rubinius::Object, Rubinius::Module, Rubinius::Class].include?(klass)
        true
      end
    end
  end

  # descendency hints
  class Numeric < ::Rubinius::Object; end
  class Float < Numeric; end
  class Integer < Numeric; end
  class Fixnum < Integer; end
  class Bignum < Integer; end
  module Errno; end
end


# load it all up
kernel_dir = File.dirname(__FILE__) + '/../../kernel'
files = Dir[kernel_dir + '/bootstrap/*.rb'].sort
files += Dir[kernel_dir + '/core/*.rb'].sort
files.delete_if {|f| f =~ /__loader/ }

# sort in case lexicographical order matters
files.each do |file|
  id = '[A-Z][A-za-z0-9]*'
  klass = "class\\s+#{id}"

  code = IO.read(file)
  code.gsub!(/#{klass}[^\S\n]*$/) do |str|
    # don't do it for Object or Class, Float, Integer, Fixnum, or Bignum
    if str =~ /^class\s+(Class|Float|Integer|Fixnum|Bignum)/
      str # do nothing
    elsif str =~ /^class\s+Object/
      str + " < ::Object"
    else
      str + " < ::Rubinius::Object"
    end
  end
  Rubinius.module_eval(code)
end

# system errors are defined in C
# can't we just define these errors in ruby CABO ?!?!
system_errors = open('shotgun/lib/bootstrap.c').read.scan(/set_syserr\((.+?),/).flatten - ['num']
system_errors.each do |name|
  Rubinius::Errno.module_eval "class #{name} < SystemCallError; end"
end

mri_modules = {}
rubinius_modules = {}
# use proc so it doesn't add to ObjectSpace
info = Proc.new do |mod|
  methods = mod.methods
  instance_methods = mod.instance_methods
  inherited_instance_methods =

  # if class then specify inherited methods
  if mod.instance_of?(Class) and mod.superclass
    inherited_instance_methods = mod.superclass.instance_methods
  end

  {:class => mod.class,
   :methods => methods,
   :instance_methods => instance_methods,
   :inherited_instance_methods => inherited_instance_methods}
end

ObjectSpace.each_object(Module) do |mod|
  if mod.to_s =~ /^Rubinius::/
    rubinius_modules[mod.to_s.split('Rubinius::')[1]] = info.call(mod)
  else
    mri_modules[mod.to_s] = info.call(mod)
  end
end

# i need to do alot of summing
class Array
  def sum
    empty? ? 0 : inject {|x, y| x + y }
  end
end

# add diffs
mri_modules.each do |name, info|
  rbn_info = rubinius_modules[name]
  if rbn_info
    # in ruby Object is the simplest form of "instance" -- its class needs to have its instance methods
    mri_modules[name][:methods_diff] = info[:methods]
    mri_modules[name][:methods_diff] -= rbn_info[:methods]
    # we want to know diff for low level classes but not for all others
    if ['Object', 'Module'].include?(name)
      mri_modules[name][:methods_diff] -= Rubinius::Object.instance_methods
      mri_modules[name][:methods_diff] -= Rubinius::Module.instance_methods
    else
      mri_modules[name][:methods_diff] -= Object.methods
      mri_modules[name][:methods_diff] -= Module.methods
    end
    mri_modules[name][:methods_diff].sort!

    mri_modules[name][:instance_methods_diff] = info[:instance_methods]
    mri_modules[name][:instance_methods_diff] -= rbn_info[:instance_methods]
    mri_modules[name][:instance_methods_diff] -= Array(info[:inherited_instance_methods])
    mri_modules[name][:instance_methods_diff] -= Array(rbn_info[:inherited_instance_methods])
    mri_modules[name][:instance_methods_diff].sort!
  end
end

unimplemented_constants = mri_modules.select do |name, info|
  info[:methods_diff].nil? and info[:instance_methods_diff].nil?
end.map {|name, info| name }

completed_constants = mri_modules.map do |name, info|
  name if info[:methods_diff] == [] and info[:instance_methods_diff] == []
end.compact

incomplete_constants = mri_modules.inject({}) do |constants, segment|
  name, info = segment

  if unimplemented_constants.include?(name) or completed_constants.include?(name)
    constants
  else
    constants.merge(name => info)
  end
end

constants_length = mri_modules.length

completed_constants_length = completed_constants.length

# rough estimation -- not *exact*
methods_length = mri_modules.
  map {|name, info| info[:methods].length - Object.methods.length }.sum +
  Object.methods.length

# rough estimation -- not *exact*
completed_methods_length = completed_constants.
  map {|name| mri_modules[name][:methods].length - Object.methods.length }.sum +
  incomplete_constants.map {|name, info| info[:methods].length - Object.methods.length }.sum +
  Object.methods.length

# rough estimation -- not *exact*
instance_methods_length = mri_modules.
  map {|name, info| info[:instance_methods].length - Array(info[:inherited_instance_methods]).length }.sum

# rough estimation -- not *exact*
completed_instance_methods_length = completed_constants.
  map {|name| mod = mri_modules[name]; mod[:instance_methods].length -
  Array(mod[:inherited_instance_methods]).length }.sum +
  incomplete_constants.map {|name, info| info[:instance_methods].length -
  Array(info[:inherited_instance_methods]).length }.sum

puts Marshal.dump({
  :unimplemented_constants => unimplemented_constants.sort,
  :incomplete_constants => incomplete_constants.sort,
  :completed_constants => completed_constants.sort,
  :constants_length => constants_length,
  :completed_constants_length => completed_constants_length,
  :methods_length => methods_length,
  :completed_methods_length => completed_methods_length,
  :instance_methods_length => instance_methods_length,
  :completed_instance_methods_length => completed_instance_methods_length
})
