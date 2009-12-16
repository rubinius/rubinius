require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/inspect'

describe "Struct#inspect" do
  it "returns a string representation of some kind" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.inspect.should == '#<struct Struct::Car make="Ford", model="Ranger", year=nil>'
    # ujihisa reported in http://rubyspec.org/issues/show/130 that the
    # following example failed under mspec. Prefixing 'Whiskey' with a double
    # colon causes it to work. Given that this is an mspec bug, as opposed to
    # a problem with a spec, I've used the workaround below.
    ::Whiskey = Struct.new(:name, :ounces)
    ::Whiskey.new('Jack', 100).inspect.should == '#<struct Whiskey name="Jack", ounces=100>'
  end
 
  it_behaves_like(:struct_inspect, :inspect)
end
