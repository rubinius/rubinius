require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each'
require 'timeout'

# force reload for Prime::method_added and Prime::instance
Object.send(:remove_const, :Prime) if defined?(Prime)
load 'prime.rb'

describe "Prime.each" do
  it_behaves_like :prime_each, :each, Prime
end

describe "Prime#each" do
  it_behaves_like :prime_each, :each, Prime.instance
end

describe "Prime#each", "when an instance created via Prime.new" do
  before(:all) do
    @enough_seconds = 3
    @primes = [
      2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37,
      41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83,
      89, 97
    ].freeze
  end

  before do
    @ps = Prime.new
  end

  it "iterates the given block over all prime numbers" do
    enumerated = []
    @ps.each do |prime|
      break if prime >= 100
      enumerated << prime
    end
    enumerated.should == @primes
  end

  it "infinitely iterates the given block passing a prime" do
    lambda {
      Timeout.timeout(@enough_seconds) {
        @ps.each {|prime| 
          primality = (2..Math.sqrt(prime)).all?{|n| prime%n != 0 }
          primality.should be_true
        }
      }
    }.should raise_error(Timeout::Error)
  end

  it "raises a ArgumentError when is called with some argumetns" do
    lambda { 
      @ps.each(100) do |prime|
        # do something
      end
    }.should raise_error(ArgumentError)
  end

  it "passes a prime to the given block ascendently" do
    prev = 1
    @ps.each do |prime|
      break if prime >= 10000
      prime.should > prev
      prev = prime
    end
  end

  it "returns an evaluated value of the given block" do
    expected = Object.new
    Prime.new.each{ break expected }.should equal(expected)
  end

  it "returns an enumerator (or a compatible object) if no block given" do
    obj = @ps.each
    obj.should be_kind_of(Enumerable)
    obj.should respond_to(:with_index)
    obj.should respond_to(:with_object)
    obj.should respond_to(:next)
    obj.should respond_to(:rewind)
  end

  it "raises an ArgumentError if no block given and is called with some arguments" do
    lambda { @ps.each(100) }.should raise_error(ArgumentError)
  end

  it "does not rewind the generator, each loop start at the current value" do
    @ps.next

    result = []
    @ps.each do |p|
      result << p
      break if p > 10
    end
    result.should == [3, 5, 7, 11]

    result = []
    @ps.each do |p|
      result << p
      break if p > 20
    end
    result.should == [13, 17, 19, 23]
  end
end
