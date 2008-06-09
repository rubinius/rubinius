require 'mini/test'

module Test
  module Unit # was ::Mini::Test, but rails' horrid code forced my hand
    if defined? TestCase then
      warn "ARGH! someone defined Test::Unit::TestCase rather than requiring"
      remove_const :TestCase
    end

    TestCase             = ::Mini::Test::TestCase
    AssertionFailedError = ::Mini::Assertion

    class TestCase
      alias :method_name :name # so lame
    end
  end
end

require 'test/unit/assertions' # brings in deprecated methods

Mini::Test.autorun
