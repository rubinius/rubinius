############################################################
# This file is imported from a different project.
# DO NOT make modifications in this repo.
# File a patch instead and assign it to Ryan Davis
############################################################

require 'minitest/unit'
require 'test/unit/deprecate'

module Test; end
module Test::Unit # was ::Mini::Test, but rails' horrid code forced my hand
  if defined? TestCase then
    warn "ARGH! someone defined Test::Unit::TestCase rather than requiring"
    remove_const :TestCase
  end

  AssertionFailedError = ::MiniTest::Assertion

  class TestCase < ::MiniTest::Unit::TestCase

    VERSION = '1.3.0'

    tu_deprecate :method_name, :name # 2009-06-01

    def self.test_order              # 2009-06-01
      :sorted
    end
  end
end

require 'test/unit/assertions' # brings in deprecated methods
