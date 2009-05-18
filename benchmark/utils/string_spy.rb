require File.dirname(__FILE__) + '/class_spy'

methods = [
  :+,
  :*,
  :%,
  :<<,
  :==,
  :<=>,
  :[],
  :[]=,
  :eql?,
  :each,
  :each_byte,
  :each_char,
  :each_with_index,
  :include?,
  :split,
  :squeeze,
  :strip,
  :to_i,
  :to_sym,
  :to_s
]

ClassSpy.new String, methods
