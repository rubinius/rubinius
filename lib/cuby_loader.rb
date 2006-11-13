require 'rubygems'
require 'cuby'
require 'sydparse'

class Cuby::Loader
  def initialize(cuby)
    @cuby = cuby
  end
  
  def load_string(str)
    syd = SydneyParser.load_string str
    @cuby.generate_from syd.sexp
  end
end