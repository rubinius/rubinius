require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#included" do
  it "is invoked when self is included in another module or class" do
    begin
      m = Module.new do
        def self.included(o)
          $included_by = o
        end
      end
      
      c = Class.new { include m }
      
      $included_by.should == c
    ensure
      $included_by = nil
    end
  end
end
