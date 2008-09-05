require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib'
require 'pt_testcase'

SEXP_EXPECTATIONS = ParseTreeTestCase.testcases

class String
  def to_sexp
    require 'ruby_parser'

    RubyParser.new.process(self, "(string)")
  end
end

describe "String#to_sexp" do
  SEXP_EXPECTATIONS.each do |node, hash|
    next if Array === hash['Ruby']

    it "parses :#{node}" do
      expected = hash['ParseTree']
      actual   = hash['Ruby'].to_sexp
      expected.should == actual
    end
  end
end
