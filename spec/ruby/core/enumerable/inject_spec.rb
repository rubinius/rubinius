require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/inject', __FILE__)

describe "Enumerable#inject" do
  it_behaves_like :enumerable_inject, :inject
  
  it "should work with enumerators that return arrays" do
    passed_values = []
    [:a].each_with_index.inject(0) do |accumulator,value|
      passed_values << value
      accumulator + 1
    end.should == 1
    passed_values.should == [[:a,0]]
  end
  
end
