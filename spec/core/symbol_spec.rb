require File.dirname(__FILE__) + '/../spec_helper'

# ===, id2name, inspect, to_i, to_int, to_s, to_sym

context "Symbol instance method" do
  specify "=== should return true if other is the same symbol" do
    example do
      @sym = :ruby
      p [@sym === :ruby, :foo === :bar, :one === 'one'.intern, :nope === 'nope', :yep === 'yep'.to_sym]
    end.should == '[true, false, true, false, true]'
  end
  
  specify "id2name should return the string corresponding to self" do
    example do
      p [:rubinius.id2name, :squash.id2name, 'string'.to_sym.id2name]
    end.should == '["rubinius", "squash", "string"]'
  end
  
  specify "inspect should return the representation of self as a symbol literal" do
    example do
      p [:ruby.inspect, :file.inspect]
    end.should == '[":ruby", ":file"]'
  end
  
  specify "to_i should return an integer for a symbol" do
    example do
      p [:ruby.to_i.kind_of?(Integer), 'rubinius'.to_sym.to_i.kind_of?(Integer)]
    end.should == '[true, true]'
  end
  
  specify "to_int should be a synonym for to_i" do
    example do
      p [:ruby.to_i.kind_of?(Integer), 'rubinius'.to_sym.to_i.kind_of?(Integer)]
    end.should == '[true, true]'
  end
  
  specify "to_s should be a synonym for id2name" do
    example do
      p [:rubinius.to_s, :squash.to_s, 'string'.to_sym.to_s]
    end.should == '["rubinius", "squash", "string"]'
  end
  
  specify "to_sym should return self" do
    example do
      p [:rubinius.to_sym, :ruby.to_sym]
    end.should == '[:rubinius, :ruby]'
  end  
end
