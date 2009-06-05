require 'matrix'
require File.dirname(__FILE__) + '/../../../fixtures/matrix'

describe :equal, :shared => true do
  before do
    @matrix = Matrix[ [1, 2, 3, 4, 5], [2, 3, 4, 5, 6] ]
  end

  it "returns true for self" do
    @matrix.eql?(@matrix).should be_true
  end

  ruby_bug("[ruby-dev:36298]", "1.8.7") do
    it "returns true when the each corresponding elements are equal in the sense of Object#eql?" do
      @matrix.eql?(Matrix[ [1, 2, 3, 4, 5], [2, 3, 4, 5, 6] ]).should be_true

      num1, num2 = TrivialField.new, TrivialField.new
      Matrix[[num1]].eql?(Matrix[[num2]]).should be_true
    end

    it "returns false when there are a pair corresponding elements which are not equal in the sense of Object#eql?" do
      @matrix.eql?(Matrix[ [1, 2, 3, 4, 5.0], [2, 3, 4, 5, 6] ]).should be_false
    end
  end
end
