require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#extend_object" do
  it "extends the given object with constants and methods of self" do
    m = Module.new do
      const_set :C, "test"
      def test() "hello" end
    end
    
    o = mock('test')
    m.send(:extend_object, o)
    
    o.test.should == "hello"
    (class << o; C; end).should == "test"
  end
  
  it "is called when an object gets extended with self" do
    begin
      m = Module.new do
        def self.extend_object(o)
          $extended_object = o
        end
      end
      
      (o = mock('x')).extend(m)
      
      $extended_object.should == o
    ensure
      $extended_object = nil
    end
  end
end
