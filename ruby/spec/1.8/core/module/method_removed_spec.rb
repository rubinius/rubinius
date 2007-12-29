require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#method_removed" do
  it "is called when a method is removed from self" do
    begin
      Module.new do
        def self.method_removed(name)
          $method_removed = name
        end
        
        def test
          "test"
        end
        remove_method :test
      end
      
      $method_removed.should == :test
    ensure
      $method_removed = nil
    end
  end
end
