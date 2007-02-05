require 'rubygems'
require 'spec'

module PrimitiveSpecHelper
  def run_code(code)
  `shotgun/rubinius -e '#{code}'`
  end

  # have to do this the long way in case literals need to be pushed
  def run_primitive(name, *args)
    declarations = ""
    instructions = ""
    num_args = args.length

    ('a'..'zzz').each do |id|
      break unless _value = args.pop
      declarations << "#{id} = #{_value.inspect}; "
      instructions << "push #{id}\n"
    end

    code = "begin; #{declarations} puts Ruby.asm(\"" +
           "#{instructions}send_primitive #{name} #{num_args}" +
           "\"); " +
           "rescue PrimitiveFailure; puts \"PrimitiveFailure\".inspect; end"
    eval run_code(code)
  end
end

class PrimitiveFailure < Exception; end

class String
  # all this does is assumes that the string _is_ the exception name
  def should_raise(exc)
    self == exc.to_s
  end
end

context "The compare primitive sent two integers" do
  include PrimitiveSpecHelper

  specify "should return 1 if x > y" do
    run_primitive(:compare, 30, 20).should == 1
    run_primitive(:compare, 10, -5).should == 1
    run_primitive(:compare, -10, -500).should == 1
  end

  specify "should return 0 if x == y" do
    run_primitive(:compare, 500, 500).should == 0
    run_primitive(:compare, 0, 0).should == 0
    run_primitive(:compare, -11, -11).should == 0
  end

  specify "should return -1 if x < y" do
    run_primitive(:compare, 3, 500).should == -1
    run_primitive(:compare, -8, 3).should == -1
    run_primitive(:compare, -1000, -20).should == -1
  end

end

context "The compare primitive sent non-integers" do
  include PrimitiveSpecHelper

  specify "should raise PrimitiveFailure if sent int, non-int" do
    run_primitive(:compare, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:compare, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure if sent non-int, int" do
    run_primitive(:compare, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:compare, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure if sent non-int, non-int" do
    run_primitive(:compare, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:compare, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:compare, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

