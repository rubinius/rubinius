require File.dirname(__FILE__) + '/../../../spec_helper'
require 'matrix'
require File.dirname(__FILE__) + '/../../../fixtures/matrix'

describe "Vector#eql?" do
  before do
    @vector = Vector[1, 2, 3, 4, 5]
  end

  it "returns true for self" do
    @vector.eql?(@vector).should be_true
  end

  ruby_bug("[ruby-dev:36298]", "1.8.7") do
    it "returns true when the each corresponding elements are equal in the sense of Object#eql?" do
      @vector.eql?(Vector[1, 2, 3, 4, 5]).should be_true

      num1, num2 = TrivialField.new, TrivialField.new
      Vector[num1].eql?(Vector[num2]).should be_true
    end

    it "returns false when there are a pair corresponding elements which are not equal in the sense of Object#eql?" do
      @vector.eql?(Vector[1, 2, 3, 4, 5.0]).should be_false
    end
  end
end
