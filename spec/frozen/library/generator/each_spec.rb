require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Generator#each" do
  it "enumerates the elements" do
    g = GeneratorSpecs.four_elems
    result = []

    g.each { |element|
      result << element
    }

    result.should == ['A', 'B', 'C', 'Z']
  end

  it "rewinds the generator and only then enumerates the elements" do
    g = GeneratorSpecs.four_elems
    g.next; g.next
    result = []

    g.each { |element|
      result << element
    }

    result.should == ['A', 'B', 'C', 'Z']
  end
end
