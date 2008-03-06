# don't define anything, this is just so we don't get the real one

$TEST_UNIT_TESTCASE_DEPRECATION ||= false

warn "require 'test/unit/testcase' has been deprecated" unless
  $TEST_UNIT_TESTCASE_DEPRECATION

$TEST_UNIT_TESTCASE_DEPRECATION = true
