require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require File.dirname(__FILE__) + '/../fixtures/classes_1.9'

ruby_version_is "1.9" do
  describe "Method#parameters" do

    it "returns an empty Array when the method expects no arguments" do
      MethodSpecs::Methods.instance_method(:zero).parameters.should == []
    end

    it "returns [[:req,:name]] for a method expecting one required argument called 'name'" do
      MethodSpecs::Methods.instance_method(:one_req).parameters.should == [[:req,:a]]
    end

    it "returns [[:req,:a],[:req,:b]] for a method expecting two required arguments called 'a' and 'b''" do
      m = MethodSpecs::Methods.instance_method(:two_req)
      m.parameters.should == [[:req,:a], [:req,:b]]
    end

    it "returns [[:block,:a]] for a method expecting one block argument called 'a'" do
      m = MethodSpecs::Methods.instance_method(:zero_with_block)
      m.parameters.should == [[:block,:block]]
    end

    it "returns [[:req,:a],[:block,:b] for a method expecting a required argument ('a') and a block argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_req_with_block)
      m.parameters.should == [[:req,:a], [:block,:block]]
    end

    it "returns [[:req,:a],[:req,:b],[:block,:c] for a method expecting two required arguments ('a','b') and a block argument ('c')" do
      m = MethodSpecs::Methods.instance_method(:two_req_with_block)
      m.parameters.should == [[:req,:a], [:req,:b], [:block,:block]]
    end

    it "returns [[:opt,:a]] for a method expecting one optional argument ('a')" do
      m = MethodSpecs::Methods.instance_method(:one_opt)
      m.parameters.should == [[:opt,:a]]
    end

    it "returns [[:req,:a],[:opt,:b]] for a method expecting one required argument ('a') and one optional argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_req_one_opt)
      m.parameters.should == [[:req,:a],[:opt,:b]]
    end

    it "returns [[:req,:a],[:opt,:b]] for a method expecting one required argument ('a') and one optional argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_req_one_opt)
      m.parameters.should == [[:req,:a],[:opt,:b]]
    end

    it "returns [[:req,:a],[:opt,:b],[:opt,:c]] for a method expecting one required argument ('a') and two optional arguments ('b','c')" do
      m = MethodSpecs::Methods.instance_method(:one_req_two_opt)
      m.parameters.should == [[:req,:a],[:opt,:b],[:opt,:c]]
    end

    it "returns [[:req,:a],[:req,:b],[:opt,:c]] for a method expecting two required arguments ('a','b') and one optional arguments ('c')" do
      m = MethodSpecs::Methods.instance_method(:two_req_one_opt)
      m.parameters.should == [[:req,:a],[:req,:b],[:opt,:c]]
    end
    
    it "returns [[:opt,:a],[:block,:b]] for a method expecting one required argument ('a') and one block argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_opt_with_block)
      m.parameters.should == [[:opt,:a],[:block,:block]]
    end

    it "returns [[:req,:a],[:opt,:b],[:block,:c]] for a method expecting one required argument ('a'), one optional argument ('b'), and a block ('c')" do
      m = MethodSpecs::Methods.instance_method(:one_req_one_opt_with_block)
      m.parameters.should == [[:req,:a],[:opt,:b],[:block,:block]]
    end

    it "returns [[:req,:a],[:opt,:b],[:opt,:c],[:block,:d]] for a method expecting one required argument ('a'), two optional arguments ('b','c'), and a block ('d')" do
      m = MethodSpecs::Methods.instance_method(:one_req_two_opt_with_block)
      m.parameters.should == [[:req,:a],[:opt,:b],[:opt,:c],[:block,:block]]
    end

    it "returns [[:rest,:a]] for a method expecting a single splat argument ('a')" do
      m = MethodSpecs::Methods.instance_method(:zero_with_splat)
      m.parameters.should == [[:rest,:a]]
    end

    it "returns [[:req,:a],[:rest,:b]] for a method expecting a splat argument ('a') and a required argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_req_with_splat)
      m.parameters.should == [[:req,:a],[:rest,:b]]
    end

    it "returns [[:req,:a],[:req,:b],[:rest,:c]] for a method expecting two required arguments ('a','b') and a splat argument ('c')" do
      m = MethodSpecs::Methods.instance_method(:two_req_with_splat)
      m.parameters.should == [[:req,:a],[:req,:b],[:rest,:c]]
    end

    it "returns [[:req,:a],[:opt,:b],[:rest,:c]] for a method expecting a required argument ('a','b'), an optional argument ('b'), and a splat argument ('c')" do
      m = MethodSpecs::Methods.instance_method(:one_req_one_opt_with_splat)
      m.parameters.should == [[:req,:a],[:opt,:b],[:rest,:c]]
    end

    it "returns [[:req,:a],[:req,:b],[:opt,:b],[:rest,:d]] for a method expecting two required arguments ('a','b'), an optional argument ('c'), and a splat argument ('d')" do
      m = MethodSpecs::Methods.instance_method(:two_req_one_opt_with_splat)
      m.parameters.should == [[:req,:a],[:req,:b],[:opt,:c],[:rest,:d]]
    end

    it "returns [[:req,:a],[:opt,:b],[:opt,:c],[:rest,:d]] for a method expecting a required argument ('a'), two optional arguments ('b','c'), and a splat argument ('d')" do
      m = MethodSpecs::Methods.instance_method(:one_req_two_opt_with_splat)
      m.parameters.should == [[:req,:a],[:opt,:b],[:opt,:c],[:rest,:d]]
    end

    it "returns [[:rest,:a],[:block,:b]] for a method expecting a splat argument ('a') and a block argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:zero_with_splat_and_block)
      m.parameters.should == [[:rest,:a],[:block,:block]]
    end

    it "returns [[:req,:a],[:rest,:b],[:block,:c]] for a method expecting a required argument ('a'), a splat argument ('b'), and a block ('c')" do
      m = MethodSpecs::Methods.instance_method(:one_req_with_splat_and_block)
      m.parameters.should == [[:req,:a],[:rest,:b],[:block,:block]]
    end

    it "returns [[:req,:a],[:req,:b],[:rest,:c],[:block,:d]] for a method expecting two required arguments ('a','b'), a splat argument ('c'), and a block ('d')" do
      m = MethodSpecs::Methods.instance_method(:two_req_with_splat_and_block)
      m.parameters.should == [[:req,:a],[:req,:b],[:rest,:c],[:block,:block]]
    end
    
    it "returns [[:req,:a],[:opt,:b],[:rest,:c],[:block,:d]] for a method expecting a required argument ('a'), a splat argument ('c'), and a block ('d')" do
      m = MethodSpecs::Methods.instance_method(:one_req_one_opt_with_splat_and_block)
      m.parameters.should == [[:req,:a],[:opt,:b],[:rest,:c],[:block,:block]]
    end

    it "returns [[:req,:a],[:req,:b],[:opt,:c],[:block,:d]] for a method expecting two required arguments ('a','b'), an optional argument ('c'), a splat argument ('d'), and a block ('e')" do
      m = MethodSpecs::Methods.instance_method(:two_req_one_opt_with_splat_and_block)
      m.parameters.should == [[:req,:a],[:req,:b],[:opt,:c],[:rest,:d],[:block,:block]]
    end

    # 1.9 semantics
    #
    it "returns [[:rest,:a],[:req,:b]] for a method expecting a splat argument ('a') and a required argument ('b')" do
      m = MethodSpecs::Methods.instance_method(:one_splat_one_req)
      m.parameters.should == [[:rest,:a],[:req,:b]]
    end
    
    it "returns [[:rest,:a],[:req,:b],[:req,:c]] for a method expecting a splat argument ('a') and two required arguments ('b','c')" do
      m = MethodSpecs::Methods.instance_method(:one_splat_two_req)
      m.parameters.should == [[:rest,:a],[:req,:b],[:req,:c]]
    end

    it "returns [[:rest,:a],[:req,:b],[:block,:c]] for a method expecting a splat argument ('a'), a required argument ('b'), and a block ('c')" do
      m = MethodSpecs::Methods.instance_method(:one_splat_one_req_with_block)
      m.parameters.should == [[:rest,:a],[:req,:b],[:block,:block]]
    end

    it "works with ->(){} as the value of an optional argument" do
      m = MethodSpecs::Methods.instance_method(:one_opt_with_stabby)
      m.parameters.should == [[:opt,:a]]
    end
  end
end
