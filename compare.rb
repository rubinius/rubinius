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

module Rubinius
end

def require_rubinius_class(klass)
  Rubinius.module_eval IO.read(File.join(File.dirname(__FILE__), 'kernel', underscore(klass) + '.rb'))
end

def class_methods_left(klass)
  eval(klass.to_s).methods - eval('Rubinius::' + klass.to_s).methods
end

def instance_methods_left(klass)
  eval(klass.to_s).instance_methods - eval('Rubinius::' + klass.to_s).instance_methods
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

ARGV.each do |klass|
  require_rubinius_class(klass)
  print_comparison(klass)
end

