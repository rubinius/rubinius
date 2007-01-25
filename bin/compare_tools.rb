# Matthew Elder <sseses@gmail.com>
# See what methods were missing on core ruby classes for rubinius

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

# load it all up
kernel_dir = File.dirname(__FILE__), '../kernel'
files = Dir[File.join(kernel_dir, '**', '*.rb')]
files.delete_if {|f| f =~ /__loader/ }
# sort in case lexicographical order matters
files.sort.each do |file|
  Rubinius.module_eval IO.read(file)
end


# heavy lifting
module CompareTools

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
  def rubinius_classes
    constants = Rubinius.constants.map {|const| Rubinius.module_eval(const) }
    constants.select {|const| Class === const }
  end

  def mri_classes
    constants = Object.constants.map {|const| Object.module_eval(const) }
    constants.select {|const| Class === const }
  end

  # high level function for output
  def complete_classes
    diffs.select {|klass, info| info[:complete] }.
      map {|klass, info| klass.to_s }.sort
  end

  # high level function for output
  def partially_complete_classes
    x = diffs.select {|klass, info| !info[:complete] }.
      map {|klass, info| [klass.to_s, info] }.
      sort {|x, y| x.first <=> y.first }
  end

  # high level function for output
  def incomplete_classes
    (mri_classes - rubinius_classes).
      map {|klass| klass.to_s }.sort
  end

  def diffs
    @diffs ||= mri_classes.inject({}) do |info, klass|
      # if the rubinius class exists, add this info
      if rklass = rubinius_class(klass)
        k_info = {}
        k_info[:method_diff] = (klass.methods - rklass.methods).sort
        k_info[:instance_method_diff] =\
          (klass.instance_methods - rklass.instance_methods).sort

        if k_info[:method_diff].empty? and k_info[:instance_method_diff].empty?
          k_info[:complete] = true
        else
          k_info[:complete] = false
        end
        info.merge(klass => k_info)
      # otherwise don't add this to diff info (not implemented)
      else
        info
      end
    end
  end
end

