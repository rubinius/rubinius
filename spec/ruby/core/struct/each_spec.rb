require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#each" do
  it "passes each value to the given block" do
    car = Struct::Car.new('Ford', 'Ranger')
    i = -1
    car.each do |value|
      value.should == car[i += 1]
    end
  end

  ruby_version_is "" ... "1.8.7" do
    it "fails if not passed a block" do
      car = Struct::Car.new('Ford', 'Ranger')
      lambda { car.each }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator if not passed a block" do
      car = Struct::Car.new('Ford', 'Ranger')
      car.each.should be_kind_of(enumerator_class)
    end
  end
end
