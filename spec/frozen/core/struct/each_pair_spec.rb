require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#each_pair" do
  it "passes each key value pair to the given block" do
    car = Struct::Car.new('Ford', 'Ranger', 2001)
    car.each_pair do |key, value|
      value.should == car[key]
    end
  end

  ruby_version_is "" ... "1.8.7" do
    it "fails if not passed a block" do
      car = Struct::Car.new('Ford', 'Ranger')
      lambda { car.each_pair }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator if not passed a block" do
      car = Struct::Car.new('Ford', 'Ranger')
      car.each_pair.should be_kind_of(enumerator_class)
    end
  end
end
