require 'mini/test'
require 'test/unit'

module Test::Unit
  if defined? Assertions then
    CRAP_ASSERTIONS = Assertions
    remove_const :Assertions

    ObjectSpace.each_object(Module) do |offender|
      offender.send :include, ::Mini::Assertions if offender < CRAP_ASSERTIONS
    end

    Test::Unit::TestCase.send :include, CRAP_ASSERTIONS
  end

  Assertions = ::Mini::Assertions

  module Assertions
    def self.included x
      x.send :include, Test::Unit::CRAP_ASSERTIONS if
        defined? Test::Unit::CRAP_ASSERTIONS
    end
  end
end
