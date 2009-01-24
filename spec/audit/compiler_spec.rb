require File.dirname(__FILE__) + '/spec_helper'
require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib'
require 'pt_testcase'

require File.dirname(__FILE__) + '/compiler_test'

describe "Compiler::*Nodes" do
  ParseTreeTestCase.testcases.sort.each do |node, hash|
    next if Array === hash['Ruby']
    next if hash['Compiler'] == :skip

    it "compiles :#{node}" do
      input    = hash['Ruby']
      expected = hash['Compiler']

      input.should_not == nil
      expected.should_not == nil

      sexp   = input.to_sexp
      comp   = Compiler.new TestGenerator
      node   = comp.convert_sexp s(:snippit, sexp)
      actual = TestGenerator.new
      node.bytecode actual

      actual.should == expected
    end
  end
end
