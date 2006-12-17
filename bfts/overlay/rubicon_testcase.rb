require "test/unit"

# AK: forked from bfts in order to remove a test/unit specific dep.

# TODO: rename this... drop rubicon lineage.
class RubiconTestCase < Test::Unit::TestCase

  VERSION = "1.0.0"

  MsWin32 = :gak # TODO: fix
  JRuby = :gak
  $os = :not_gak

  def ruby_version
    defined?(RUBY_VERSION) ? RUBY_VERSION : VERSION
  end

  def test_nathanial_talbott_is_my_archenemy
    # do nothing but appease nathanial's inability to envision
    # abstract test classes... stabity stab stab
  end

  # TODO: this is overly complicated and dumb
  def truth_table(method, *result)
    for a in [ false, true ]
      expected = result.shift
      assert_equal(expected, method.call(a))
      assert_equal(expected, method.call(a ? self : nil))
    end
  end
end
