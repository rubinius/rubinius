require 'rubygems'
require 'parse_tree'
require 'test/unit'
require 'spec'
require 'fileutils'
$:.unshift(File.dirname(__FILE__) + '/../lib')

# Definitions
#
# spec: a statement of an expected behavior
# example: a segment of code that illustrates a spec and that can pass or fail an expectation
# host: the system (interpreter, vm, etc.) that will execute the spec frameworkd (e.g. RSpec)
# target: the system (interpreter , vm, etc.) on which the example will be executed

class RubyTarget < Test::Unit::TestCase
  def example(&block)
    yield
  end
end

class RubiniusTargetError < RuntimeError; end

# The big idea:
# 1) Using ParseTree, convert the block passed to example to an sexp
#   a) Create a new class
#   b) Define a method on the class that is the block as a Proc
#   c) Parse the class into an sexp
#   d) Extract from the sexp only the sexp for the block body
# 2) Pass the sexp for the block body to the ByteCode::Compiler
# 3) Pass the compiled script to the Machine to execute
# 4) Marshal the return value to a file (put this in the block?)
# 5) In the Host, read in the marshalled object
# 6) In the Host, complete execution of spec expectation (e.g. .should == <blah>)
class RubiniusTarget < Test::Unit::TestCase
  def example(&block)
    klass = Class.new
    klass.send(:define_method, :example, block)
    find_block_sexp(ParseTree.new.parse_tree(klass))
  end
  
  private
  def find_block_sexp(sexp)
    sexp
  end
end

class SpecConfiguration
  Target = RubyTarget
end

module Kernel
  def context_with_target(name, &block)
    context_without_target name do
      inherit SpecConfiguration::Target
      self.class_eval &block
    end
  end
  
  alias context_without_target context
  alias context context_with_target
end
