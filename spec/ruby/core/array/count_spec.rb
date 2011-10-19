require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#count" do
  it 'returns count of elements' do
    [1, :two, 'three'].count.should == 3
  end

  it 'returns count of elements that equals given object' do
    [1, 'some text', 'other text', 2, 1].count(1).should == 2
  end

  it 'returns count of element yielding a true value of block' do
    (1..20).to_a.count do |el|
      el % 2 == 0
    end.should == 10
  end
end
