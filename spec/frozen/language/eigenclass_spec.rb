require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/class'

describe "self in an eigenclass body (class << obj)" do
  it "is TrueClass for true" do
    class << true; self; end.should == TrueClass
  end

  it "is FalseClass for false" do
    class << false; self; end.should == FalseClass
  end

  it "is NilClass for nil" do
    class << nil; self; end.should == NilClass
  end

  it "raises a TypeError for Fixnum's" do
    1.should be_kind_of(Fixnum)
    lambda { class << 1; self; end }.should raise_error(TypeError)
  end

  it "raises a TypeError for symbols" do
    lambda { class << :symbol; self; end }.should raise_error(TypeError)
  end

  it "is a singleton Class instance" do
    mock = mock('x')
    cls = class << mock; self; end
    cls.is_a?(Class).should == true
    cls.should_not equal(Object)
    mock.is_a?(cls).should be_true
  end

  ruby_version_is "1.9" do
    it "is a metaclass for classes" do
      cls = class << ClassSpecs::A; self; end
      cls.is_a?(class << Class; self end).should == true
      cls.is_a?(Object).should == true
      cls.should_not equal(Class)
      ClassSpecs::A.is_a?(cls).should be_true
    end

    it "is a metametaclass for metaclasses" do
      klass = class << ClassSpecs::A; self end
      cls = class << klass; self end
      cls.is_a?(class << Class; class << self; self end end).should == true
      cls.is_a?(class << Class; self end).should == true
      cls.is_a?(Class)
      cls.is_a?(Object).should == true
      cls.should_not equal(Class)
      cls.should_not equal(class << Class; self end)
      klass.is_a?(cls).should be_true
    end

    it "has the class's metaclass as superclass for classes" do
      cls = class << ClassSpecs::K; self end
      cls.superclass.should == class << ClassSpecs::H; self end
    end

    it "has Class as superclass for BasicObject class" do
      cls = class << BasicObject; self end
      cls.superclass.should == Class
    end

    it "has the class's metaclass's metaclass as superclass for metaclasses" do
      cls = class << ClassSpecs::K; class << self; self end end
      cls.superclass.should == class << ClassSpecs::H; class << self; self end end
    end

    it "has the metaclass of Class as superclass for the metaclass of BasicObject" do
      cls = class << BasicObject; class << self; self end end
      cls.superclass.should == class << Class; self end
    end

    it "has the object's class as superclass" do
      cls = class << "blah"; self; end
      cls.superclass.should == String
    end

    ruby_bug("#601", "1.9") do
      it "raises a TypeError for Bignum's" do
        (1<<1024).should be_kind_of(Bignum)
        lambda { class << (1<<1024); self; end }.should raise_error(TypeError)
      end
    end
  end

  deviates_on(:rubinius) do 
    it "is a MetaClass instance" do
      cls = class << mock('x'); self; end
      cls.is_a?(MetaClass).should == true
    end

  end
end

describe "A constant on an eigenclass" do
  before(:each) do
    @object = Object.new
    class << @object
      CONST = self
    end
  end

  it "can be accessed after the eigenclass body is reopened" do
    class << @object
      CONST.should == self
    end
  end

  it "can be accessed via self::CONST" do
    class << @object
      self::CONST.should == self
    end
  end

  it "can be accessed via const_get" do
    class << @object
      const_get(:CONST).should == self
    end
  end

  it "is not defined on the object's class" do
    @object.class.const_defined?(:CONST).should be_false
  end

  it "is not defined in the eigenclass opener's scope" do
    class << @object
      CONST
    end
    lambda { CONST }.should raise_error(NameError)
  end

  it "cannot be accessed via object::CONST" do
    lambda do
      @object::CONST
    end.should raise_error(TypeError)
  end

  it "raises a NameError for anonymous_module::CONST" do
    @object = Class.new
    class << @object
      CONST = 100
    end
    
    lambda do
      @object::CONST
    end.should raise_error(NameError)
  end

  ruby_version_is "" ... "1.9" do
    it "appears in the eigenclass constant list" do
      constants = class << @object; constants; end 
      constants.should include("CONST")
    end
  end

  ruby_version_is "1.9" do
    it "appears in the eigenclass constant list" do
      constants = class << @object; constants; end 
      constants.should include(:CONST)
    end
  end

  it "does not appear in the object's class constant list" do
    @object.class.constants.should_not include(:CONST)
  end

  it "is not preserved when the object is duped" do
    @object = @object.dup

    lambda do
      class << @object; CONST; end
    end.should raise_error(NameError)
  end

  it "is preserved when the object is cloned" do
    @object = @object.clone

    class << @object
      CONST.should_not be_nil
    end
  end
end

# TODO: write these for 1.8.
ruby_version_is "1.9" do
  describe "Instance methods of an eigenclass" do
    it "includes ones of the object's class" do
      (class << ClassSpecs::K.new; self end).instance_methods.should include(:example_instance_method)
    end

    it "does not include class methods of the object's class" do
      (class << ClassSpecs::K.new; self end).instance_methods.should_not include(:example_class_method)
    end

    it "includes instance methods of Object" do
      (class << ClassSpecs::A.new; self end).instance_methods.should include(:example_instance_method_of_object)
    end

    it "does not include class methods of Object" do
      (class << ClassSpecs::A.new; self end).instance_methods.should_not include(:example_class_method_of_object)
    end

    it "includes instance methods of Class, for a class" do
      (class << ClassSpecs::A; self end).instance_methods.should include(:example_instance_method_of_class)
    end

    it "does not include class methods of Class, for a class" do
      (class << ClassSpecs::A; self end).instance_methods.should_not include(:example_class_method_of_class)
    end

    it "does not include instance methods of the metaclass of Class, for a class" do
      (class << ClassSpecs::A; self end).instance_methods.should_not include(:example_instance_method_of_metaclass)
    end

    it "does not include class methods of the metaclass of Class, for a class" do
      (class << ClassSpecs::A; self end).instance_methods.should_not include(:example_class_method_of_metaclass)
    end

    it "includes instance methods of the metaclass of Class, for a metaclass" do
      metaclass = class << ClassSpecs::A; self end
      (class << metaclass; self end).instance_methods.should include(:example_instance_method_of_metaclass)
    end

    it "does not include class methods of the metaclass of Class, for a metaclass" do
      metaclass = class << ClassSpecs::A; self end
      (class << metaclass; self end).instance_methods.should_not include(:example_class_method_of_metaclass)
    end
  end

  describe "Class methods of an eigenclass" do
    it "includes ones of the object's class" do
      (class << ClassSpecs::K.new; self end).methods.should include(:example_class_method)
    end

    it "does not include instance methods of the object's class" do
      (class << ClassSpecs::K.new; self end).methods.should_not include(:example_instance_method)
    end

    it "includes instance methods of Class" do
      (class << ClassSpecs::A.new; self end).methods.should include(:example_instance_method_of_class)
    end

    it "does not include class mtehods of Class" do
      (class << ClassSpecs::A.new; self end).methods.should_not include(:example_class_method_of_class)
    end

    it "includes instance methods of Class, for a class" do
      (class << ClassSpecs::A; self end).methods.should include(:example_instance_method_of_class)
    end

    it "includes class methods of Class, for a class" do
      (class << ClassSpecs::A; self end).methods.should include(:example_class_method_of_class)
    end

    it "includes instance methods of the metaclass of Class, for a class" do
      (class << ClassSpecs::A; self end).methods.should include(:example_instance_method_of_metaclass)
    end

    it "does not include class methods of the metaclass of Class, for a class" do
      (class << ClassSpecs::A; self end).methods.should_not include(:example_class_method_of_metaclass)
    end

    it "includes instance methods of the metaclass of Class, for a metaclass" do
      metaclass = class << ClassSpecs::A; self end
      (class << metaclass; self end).methods.should include(:example_instance_method_of_metaclass)
    end

    it "includes class methods of the metaclass of Class, for a metaclass" do
      metaclass = class << ClassSpecs::A; self end
      (class << metaclass; self end).methods.should include(:example_class_method_of_metaclass)
    end
  end
end
