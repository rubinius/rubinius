# A FileAccessor
# * should open a file and pass it to the processor's process method
# Greeter
# * should say Hi to person
# * should say Hi to nobody
# a context with helper a method
# * should make that method available to specs
# An IoProcessor
# * should raise nothing when the file is exactly 32 bytes
# * should raise an exception when the file length is less than 32 bytes
# A consumer of a mock
# * should be able to send messages to the mock
# a mock
# * should be able to mock the same message twice w/ different args
# * should be able to mock the same message twice w/ different args in reverse order
# A partial mock
# * should work at the class level
# * should revert to the original after each spec
# * can be mocked w/ ordering
# BDD framework
# * should be adopted quickly
# * should be intuitive
# State created in context_setup
# * should be accessible from spec
# * should not have sideffects
# A stack which is neither empty nor full
# * should add to the top when sent 'push'
# * should return the top item when sent 'peek'
# * should NOT remove the top item when sent 'peek'
# * should return the top item when sent 'pop'
# * should remove the top item when sent 'pop'
# An empty stack
# * should be empty
# * should no longer be empty after 'push'
# * should complain when sent 'peek'
# * should complain when sent 'pop'
# An almost empty stack (with one item)
# * should not be empty
# * should remain not empty after 'peek'
# * should become empty after 'pop'
# An almost full stack (with one item less than capacity)
# * should not be full
# * should become full when sent 'push'
# A full stack
# * should be full
# * should remain full after 'peek'
# * should no longer be full after 'pop'
# * should complain on 'push'
# A consumer of a stub
# * should be able to stub methods on any Object
# A stubbed method on a class
# * should return the stubbed value
# * should revert to the original method after each spec
# * can stub! and mock the same message
# A mock
# * can stub!
# * can stub! and mock
# * can stub! and mock the same message
# RSpec should integrate with Test::Unit::TestCase
# * TestCase#setup should be called.
# * RSpec should be able to access TestCase methods
# * RSpec should be able to accept included modules
