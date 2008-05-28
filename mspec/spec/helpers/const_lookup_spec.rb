require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/const_lookup'

module ConstLookupSpecs
  class A
    class B
    end
  end
end

describe Kernel, "#const_lookup" do
  it "returns the constant specified by 'A::B'" do
    const_lookup("ConstLookupSpecs::A::B").should == ConstLookupSpecs::A::B
  end

  it "returns a regular constant specified without scoping" do
    const_lookup("ConstLookupSpecs").should == ConstLookupSpecs
  end
end
