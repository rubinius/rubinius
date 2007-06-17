str = '"hello #{nil}"'

require 'rubygems'
require 'sydparse'

syd = SydneyParser.load_string str
p syd.sexp
