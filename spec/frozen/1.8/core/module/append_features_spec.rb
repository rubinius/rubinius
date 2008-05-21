require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#append_features" do
  it "gets called when self is included in another module/class" do
    begin
      m = Module.new do
        def self.append_features(mod)
          $appended_to = mod
        end
      end
      
      c = Class.new do
        include m
      end
      
      $appended_to.should == c
    ensure
      $appended_to = nil
    end
  end
end
