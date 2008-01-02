require File.dirname(__FILE__) + '/../spec_helper'

def util_build_generators
  @empty_generator = Generator.new([])
  @generator_one_elem = Generator.new([1])
  @generator_two_elems = Generator.new([1, 2])
  @four_elements = ['A', 'B', 'C', 'Z']
  @generator_four_elems = Generator.new(@four_elements)
end

describe "Generotar.new" do
  require 'generator'

  it "creates a new generator from an Enumerable object" do
    g = Generator.new(['A', 'B', 'C', 'Z'])
    g.should_not == nil
    g.kind_of?(Generator).should == true
  end

  it "creates a new generator from a block" do
    g = Generator.new { |g|
      for i in 'A'..'C'
        g.yield i
      end
      g.yield 'Z'
    }

    g.should_not == nil
    g.kind_of?(Generator).should == true
  end
end

describe "Generator#next?" do
  before(:each) do
    util_build_generators
  end

  it "returns false for empty generator" do
    @empty_generator.next?.should == false
  end

  it "returns true for non-empty generator" do
    g = Generator.new([1])
    g.next?.should == true

    @generator_two_elems.next?.should == true

    g = Generator.new(['A', 'B', 'C', 'D', 'E', 'F'])
    g.next?.should == true
  end

  it "returns true if the generator has not reached the end yet" do
    @generator_two_elems.next
    @generator_two_elems.next?.should == true
  end

  it "returns false if the generator has reached the end" do
    g = @generator_two_elems
    g.next
    g.next
    g.next?.should == false
  end

  it "returns false if end? returns true" do
    g = @generator_two_elems
    def g.end?; true end
    g.next?.should == false
  end
end

describe "Generator#next" do
  before(:each) do
    util_build_generators
  end

  it "raises EOFError on empty generator" do
    lambda { @empty_generator.next }.should raise_error(EOFError)
  end

  it "raises EOFError if no elements available" do
    g = @generator_two_elems
    g.next;
    g.next
    lambda { g.next }.should raise_error(EOFError)
  end

  it "raises EOFError if end? returns true" do
    g = @generator_two_elems
    def g.end?; true end
    lambda { g.next }.should raise_error(EOFError)
  end

  it "returns the element at the current position and moves forward" do
    g = @generator_two_elems
    g.index.should == 0
    g.next.should == 1
    g.index.should == 1
  end

  it "subsequent calls should return all elements in proper order" do
    g = @generator_four_elems

    result = []
    while g.next?
      result << g.next
    end

    result.should == @four_elements
  end 
end

describe "Generator#rewind" do
  before(:each) do
    util_build_generators
  end

  it "does nothing for empty generator" do
    @empty_generator.index.should == 0
    @empty_generator.rewind
    @empty_generator.index.should == 0
  end

  it "rewinds the generator" do
    g = @generator_two_elems
    orig = g.next
    g.index.should == 1
    g.rewind
    g.index.should == 0
    g.next.should == orig
  end

  it "rewinds the previously finished generator" do
    g = @generator_two_elems
    g.next; g.next
    g.rewind
    g.end?.should == false
    g.next?.should == true
    g.next.should == 1
  end
end

describe "Generator#each" do
  before(:each) do
    util_build_generators
  end

  it "enumerates the elements" do
    g = @generator_four_elems
    result = []

    g.each { |element|
      result << element
    }

    result.should == @four_elements
  end

  it "rewinds the generator and only then enumerates the elements" do
    g = @generator_four_elems
    g.next; g.next
    result = []

    g.each { |element|
      result << element
    }

    result.should == @four_elements
  end
end
