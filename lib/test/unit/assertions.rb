require 'mini/test'
require 'test/unit'

module Test::Unit
  remove_const :Assertions if defined? Assertions # nope... fuck you
  Assertions = ::Mini::Assertions
end
