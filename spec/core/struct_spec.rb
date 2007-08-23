require File.dirname(__FILE__) + '/../spec_helper'

# Class methods: new

# Generated class methods: new, [], members

# Generated class instance methods: length, size, members, to_a, 
#   values, values_at, [], []=,  each, each_pair, reader, writer,
#   inspect, ==

describe "Struct class methods" do
  failure :rubinius do
    specify "new with string as first argument should create a constant in Struct namespace" do
      # TODO: should pass, rbx bug
      Struct.new('Animal', :name, :legs, :eyeballs).should == Struct::Animal

      struct = Struct.new('Animal', :name, :legs, :eyeballs)
      struct.should == Struct::Animal
    end
  end

  specify "new with string as first argument should overwrite previously defined constants" do
    first = Struct.new('Person', :height, :weight)
    first.should == Struct::Person

    second = Struct.new('Person', :hair, :sex)
    second.should == Struct::Person

    first.members.should_not == second.members
  end

  specify "new should call to_str on its first argument (constant name)" do
    obj = Object.new
    def obj.to_str() "Foo" end
    Struct.new(obj).should == Struct::Foo
  end

  specify "new should accept nil first argument for not creating constants" do
    Struct.new(nil, :foo).new("foo").foo.should == "foo"
  end

  specify "new with symbol as first argument should not create a constant" do
    Struct.new(:Animal, :name, :legs, :eyeballs).should_not == Struct::Animal
  end

  specify "new with symbol arguments should create a new anonymous class" do
    Struct.new(:make, :model).class.should == Class
  end

  specify "new with bad constant name string as first argument should fail" do
    should_raise(NameError) { Struct.new('animal', :name, :legs, :eyeballs) }
  end

  specify "new should only accept symbols" do
    # i think this is an ArgumentError because 1.to_sym => nil, but not sure
    should_raise(ArgumentError) { Struct.new(:animal, 1) }

    should_raise(TypeError) { Struct.new(:animal, 1.0) }
    should_raise(TypeError) { Struct.new(:animal, Time.now) }
    should_raise(TypeError) { Struct.new(:animal, Class) }
    should_raise(TypeError) { Struct.new(:animal, nil) }
    should_raise(TypeError) { Struct.new(:animal, true) }
    should_raise(TypeError) { Struct.new(:animal, ['chris', 'evan']) }
    should_raise(TypeError) { Struct.new(:animal, { :name => 'chris' }) }
  end

  specify "new should instance_eval a passed block" do
    klass = Struct.new(:something) { @something_else = 'something else entirely!' }
    klass.instance_variables.should_include '@something_else'
  end
end

describe "Struct subclass" do
  class Apple < Struct; end

  specify "new should create a constant in subclass' namespace" do
    Apple.new('Computer', :size).should == Apple::Computer
  end
end


describe "Struct anonymous class class methods" do
  failure :rbx do
    Struct.new('Ruby', :version, :platform)
  end

  specify "new should create an instance" do
    Struct::Ruby.new.kind_of?(Struct::Ruby).should == true
  end

  specify "new should create reader methods" do
    Struct::Ruby.new.methods.should_include 'version'
    Struct::Ruby.new.methods.should_include 'platform'
  end

  specify "new should create writer methods" do
    Struct::Ruby.new.methods.should_include 'version='
    Struct::Ruby.new.methods.should_include 'platform='
  end

  specify "new with too many arguments should fail" do
    should_raise(ArgumentError) { Struct::Ruby.new('2.0', 'i686', true) }
  end

  specify "[] should be a synonym for new" do
    Struct::Ruby['2.0', 'i686'].class.should == Struct::Ruby
  end

  specify "members should return an array of attribute names" do
    Struct::Ruby.members.should == %w(version platform)
  end
end

describe "Struct anonymous class instance methods" do
  failure :rbx do
    Struct.new('Car', :make, :model, :year)
  end

  specify "length should return the number of attributes" do
    Struct::Car.new('Cadillac', 'DeVille').length.should == 3
    Struct::Car.new.length.should == 3
  end

  specify "size should be a synonym for length" do
    Struct::Car.new.size.should == Struct::Car.new.length
  end

  specify "members should return an array of attribute names" do
    Struct::Car.new.members.should == %w(make model year)
    Struct::Car.new('Cadillac').members.should == %w(make model year)
  end

  specify "to_a should return the values for this instance as an array" do
    Struct::Car.new('Geo', 'Metro', 1995).to_a.should == ['Geo', 'Metro', 1995]
    Struct::Car.new('Ford').to_a.should == ['Ford', nil, nil]
  end

  specify "values should be a synonym for to_a" do
    car = Struct::Car.new('Nissan', 'Maxima')
    car.values.should == car.to_a

    Struct::Car.new.values.should == Struct::Car.new.to_a
  end

  specify "values_at should return an array of values" do
    clazz = Struct.new(:name, :director, :year)
    movie = clazz.new('Sympathy for Mr. Vengence', 'Chan-wook Park', 2002)
    movie.values_at(0, 1).should == ['Sympathy for Mr. Vengence', 'Chan-wook Park']
    movie.values_at(0..2).should == ['Sympathy for Mr. Vengence', 'Chan-wook Park', 2002]
  end

  specify "values_at should fail when passed unsupported types" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(TypeError) { car.values_at('name', 'director') }
    should_raise(TypeError) { car.values_at(Class) }
    should_raise(IndexError) { car.values_at(:name, :director) }
  end

  specify "[] should return the attribute referenced" do
    car = Struct::Car.new('Ford', 'Ranger')
    car['make'].should == 'Ford'
    car['model'].should == 'Ranger'
    car[:make].should == 'Ford'
    car[:model].should == 'Ranger'
    car[0].should == 'Ford'
    car[1].should == 'Ranger'
  end

  specify "[] should fail when it doesnt know about the requested attribute" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(IndexError) { car[5] }
    should_raise(NameError) { car[:body] }
    should_raise(NameError) { car['wheels'] }
  end

  specify "[] should fail if passed too many arguments" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(ArgumentError) { car[:make, :model] }
  end

  specify "[] should fail if not passed a string, symbol, or integer" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(TypeError) { car[Time.now] }
    should_raise(TypeError) { car[ { :name => 'chris' } ] }
    should_raise(TypeError) { car[ ['chris', 'evan'] ] }
    should_raise(TypeError) { car[ Class ] }
  end

  specify "[]= should assign the passed value" do
    car = Struct::Car.new('Ford', 'Ranger')
    car[:model] = 'Escape'
    car[:model].should == 'Escape'
    car[1] = 'Excursion'
    car[:model].should == 'Excursion'
  end

  specify "[]= should fail when trying to assign attributes which don't exist" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(NameError) { car[:something] = true }
    should_raise(NameError) { car[:dogtown] = true }
    should_raise(IndexError) { car[100] = true }
    should_raise(TypeError) { car[Time.now] = true }
    should_raise(TypeError) { car[Class] = true }
  end

  specify "each should pass each value to the given block" do
    car = Struct::Car.new('Ford', 'Ranger')
    i = -1
    car.each do |value|
      value.should == car[i += 1]
    end
  end

  specify "each should fail if not passed a block" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(LocalJumpError) { car.each }
  end

  specify "each_pair should pass each key value pair to the given block" do
    car = Struct::Car.new('Ford', 'Ranger', 2001)
    car.each_pair do |key, value|
      value.should == car[key]
    end
  end

  specify "each_pair should fail if not passed a block" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(LocalJumpError) { car.each_pair }
  end

  specify "Enumerable methods should work" do
    car = Struct::Car.new('Ford', 'Ranger', '2001')
    car.detect { |value| value.include? 'F' }.should == 'Ford'
    car.reject { |value| value.include? 'F' }.should == ['Ranger', '2001']
  end

  specify "reader method should be a synonym for []" do
    klass = Struct.new(:clock, :radio)
    alarm = klass.new(true)
    alarm.clock.should == alarm[:clock]
    alarm.radio.should == alarm['radio']
  end

  specify "reader method should not interfere with undefined methods" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(NoMethodError) { car.something_weird }
  end

  specify "writer method be a synonym for []=" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.model.should == 'Ranger'
    car.model = 'F150'
    car.model.should == 'F150'
    car[:model].should == 'F150'
    car['model'].should == 'F150'
    car[1].should == 'F150'
  end

  specify "inspect should return a string representation of some kind" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.inspect.should == '#<struct Struct::Car make="Ford", model="Ranger", year=nil>'
    Whiskey = Struct.new(:name, :ounces)
    Whiskey.new('Jack', 100).inspect.should == '#<struct Whiskey name="Jack", ounces=100>'
  end

  specify "to_s should be a synonym for inspect" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.inspect.should == car.to_s
  end

  specify "== should compare all attributes" do
    Struct::Ruby.new('2.0', 'i686').should == Struct::Ruby.new('2.0', 'i686')
  end
end
