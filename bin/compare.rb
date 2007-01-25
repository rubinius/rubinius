# Matthew Elder <sseses@gmail.com>
# See what methods were missing on core ruby classes for rubinius

# stolen blatantly from activesupport
def underscore(camel_cased_word)
  camel_cased_word.to_s.gsub(/::/, '/').
    gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').
    gsub(/([a-z\d])([A-Z])/,'\1_\2').
    tr("-", "_").
    downcase
end

## Start dummy methods so mri understands rubinius core classes
class Ruby
  def primitive(*args)
  end

  def asm(*args)
  end
end

class Object
  def ivar_as_index(*args)
  end

  def instance_fields=(*args)
  end

  def index_reader(name, idx)
  end

  def define_fields(*args)
  end
end

# end dummy methods

# module to hold rubinius core namespace
module Rubinius
end

def require_rubinius_class(klass)
  Rubinius.module_eval IO.read(File.join(File.dirname(__FILE__), '../kernel', underscore(klass) + '.rb'))
end

def mri_class(klass)
  eval(klass.to_s)
rescue NameError
  nil
end

def rubinius_class(klass)
  eval('Rubinius::' + klass.to_s)
rescue NameError
  nil
end

def class_methods_left(klass)
  if mri_class = mri_class(klass)
    mri_class.methods - rubinius_class(klass).methods
  else
    []
  end
end

def instance_methods_left(klass)
  if mri_class = mri_class(klass)
    mri_class.instance_methods - rubinius_class(klass).instance_methods
  else
    []
  end
end

def print_comparison(klass)
  cmethods = class_methods_left(klass).sort
  imethods = instance_methods_left(klass).sort
  unless cmethods.empty? and imethods.empty?
    title = "::::#{klass.to_s}"
    puts title + ":" * (80 - title.length)
    puts 'Class Methods Left: ' + cmethods.join(', ')
    puts 'Instance Methods Left: ' + imethods.join(', ')
    puts ":" * 80
  end
end

kernel_dir = File.dirname(__FILE__), '../kernel'
files = Dir[File.join(kernel_dir, '**', '*.rb')]
files.delete_if {|f| f =~ /__loader/ }

files.sort.each do |file|
  Rubinius.module_eval IO.read(file)
end

classes = Rubinius.constants.select {|const| Class === Rubinius.module_eval(const) }
classes.each do |klass|
  print_comparison(klass)
end

classes_needed = Object.constants - Rubinius.constants
classes_needed = classes_needed.select {|const| Class === eval(const) }
puts
puts 'Classes that need implementing:'
puts classes_needed.sort
