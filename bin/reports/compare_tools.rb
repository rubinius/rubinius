# Matthew Elder <sseses@gmail.com>
# See what methods were missing on core ruby classes for rubinius

# dummy Object methods so mri can understand
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
  # fake these
  module Ruby
    def self.primitive(*args)
    end

    def self.asm(*args)
    end
  end

  class Numeric; end
  class Integer < Numeric; end
end

# load it all up
kernel_dir = File.dirname(__FILE__), '../../kernel'
files = Dir[File.join(kernel_dir, '**', '*.rb')]
files.delete_if {|f| f =~ /__loader/ }
# sort in case lexicographical order matters
files.sort.each do |file|
  Rubinius.module_eval IO.read(file)
end


# heavy lifting
module CompareTools
  def total_classes
    mri_classes.length
  end

  def rubinius_classes
    constants = Rubinius.constants.map {|const| Rubinius.module_eval(const) }
    constants.select {|const| Class === const }
  end

  def mri_classes
    constants = Object.constants.map {|const| Object.module_eval(const) }
    constants.select {|const| Class === const }
  end

  def mri_instance_methods_total
    mri_classes.map {|klass| x = klass.instance_methods.length }.
      inject {|x, y| x + y }
  end

  def mri_class_methods_total
    mri_classes.map {|klass| klass.methods.length }.
      inject {|x, y| x + y }
  end

  def instance_methods_left
    mri_classes.map do |klass|
      if rb_klass = rubinius_class(klass)
        # diff the methods and count whats left
        (klass.instance_methods - rb_klass.instance_methods).length
      else
        # no rb_klass so just add the mri classes' count
        klass.instance_methods.length
      end
    end.inject {|x, y| x + y }
  end

  def class_methods_left
    mri_classes.map do |klass|
      if rb_klass = rubinius_class(klass)
        # diff the methods and count whats left
        (klass.methods - rb_klass.methods).length
      else
        # no rb_klass so just add the mri classes' count
        klass.methods.length
      end
    end.inject {|x, y| x + y }
  end

  # high level function for output
  # returns a hash of classes and their diffs
  def partially_complete_classes
    class_diffs.inject({}) do |classes, segment|
      klass, info = segment
      if info[:instance_methods].empty? and info[:class_methods].empty?
        classes
      else
        classes.merge(klass => info) # incomplete
      end
    end.to_a.sort
  end

  # a classes that are finished (all methods implemented)
  # returns a list
  def complete_classes
    class_diffs.map do |klass, info|
      if info[:instance_methods].empty? and info[:class_methods].empty?
        klass # complete
      else
        nil
      end
    end.compact.sort
  end

  # classes that are not even in objectspace
  # returns a list
  def incomplete_classes
    all = mri_classes.map {|c| c.to_s }
    present = class_diffs.keys
    (all - present).sort
  end

  # diff present classes in rubinius core
  def class_diffs
    @class_diffs ||= mri_classes.inject({}) do |classes, klass|
      if rbn_klass = rubinius_class(klass)
        classes.merge( klass.to_s => class_diff(klass, rbn_klass) )
      else
        classes
      end
    end
  end

  private
  # generic class diffing algorithm
  def class_diff(klass1, klass2)
    {
      :instance_methods => (klass1.instance_methods - klass2.instance_methods).sort,
      :class_methods => (klass1.methods - klass2.methods).sort
    }
  end

  def mri_class(klass)
    eval('::' + klass.to_s)
  rescue NameError
    nil
  end

  def rubinius_class(klass)
    eval('::' + 'Rubinius::' + klass.to_s)
  rescue NameError
    nil
  end
end

