require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#const_missing" do
  it "is invoked when an undefined constant is referenced in the scope of self" do
    begin
      A = Module.new do
        const_set(:A, "test")
        
        def self.const_missing(mod)
          $missing_const = mod
          return 123
        end
      end

      A::NotExistant.should == 123
      $missing_const.should == :NotExistant
      
      A.const_get(:NotExistantToo).should == 123
      $missing_const.should == :NotExistantToo

      A::A.should == "test"
      A.const_get(:A).should == "test"
      
      $missing_const.should == :NotExistantToo
    ensure
      $missing_const = nil
    end
  end
end
