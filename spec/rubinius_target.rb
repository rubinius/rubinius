require 'test/unit'

class RubiniusTargetError < RuntimeError; end

class RubiniusTarget < Test::Unit::TestCase
  def example(&block)
    klass = Class.new
    klass.send(:define_method, :__example__, block)
    find_block_sexp(ParseTree.new.parse_tree(klass))
  end
  
  private
  def find_block_sexp(sexp)
    sexp
  end
end

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

