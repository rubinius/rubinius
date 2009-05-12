require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Binding#eval" do
  ruby_version_is '1.8.7' do
    it "behaves like Kernel.eval(..., self)" do
      obj = BindingSpecs::Demo.new(1)
      bind1 = obj.get_binding
      bind2 = obj.dup.get_binding
      { "@secret += square(3)" => 10,
        "a" => true,
        "class Inside ; end" => nil,
        "Inside.name" => "BindingSpecs::Demo::Inside"
      }.each do |test, result|
        r1 = bind1.eval(test)
        r2 = Kernel.eval(test, bind2)
        r1.should == r2
        r1.should == result
      end
    end
  
    it "needs to be completed"
  end
end
