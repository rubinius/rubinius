require File.dirname(__FILE__) + '/../spec_helper'

$: << 'lib'
require 'pt_testcase'

class String

  ##
  # This form doesn't do the rubinius specific rewriting...  That's
  # fine for now, we don't need to test that here as the compiler is
  # all over that.
  def to_sexp
    require 'ruby_parser'

    RubyParser.new.process(self, "(string)")
  end
end

describe "String#to_sexp" do
  ParseTreeTestCase.testcases.each do |node, hash|
    next if Array === hash['Ruby']

    it "parses :#{node}" do
      expected = hash['ParseTree']
      actual   = hash['Ruby'].to_sexp
      expected.should == actual
    end
  end
end
