require File.dirname(__FILE__) + '/class_spy'

methods = [
  :[],
  :[]=,
  :<<,
  :==,
  :<=>,
  :each,
  :eql?,
  :first,
  :include?,
  :last,
  :push,
  :shift
]

ClassSpy.new Array, methods
