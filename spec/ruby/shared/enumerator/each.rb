# -*- encoding: us-ascii -*-

describe :enum_each, :shared => true do
  before(:each) do
    object_each_with_arguments = Object.new
    def object_each_with_arguments.each_with_arguments(arg, *args)
      yield arg, *args
      :method_returned
    end

    @enum_with_arguments = object_each_with_arguments.to_enum(:each_with_arguments, :arg0, :arg1, :arg2)
  end

  it "yields each element of self to the given block" do
    acc = []
    enumerator_class.new([1,2,3]).each {|e| acc << e }
    acc.should == [1,2,3]
  end

  it "calls #each on the object given in the constructor by default" do
    each = mock('each')
    each.should_receive(:each)
    enumerator_class.new(each).each {|e| e }
  end

  it "calls #each on the underlying object until it's exhausted" do
    each = mock('each')
    each.should_receive(:each).and_yield(1).and_yield(2).and_yield(3)
    acc = []
    enumerator_class.new(each).each {|e| acc << e }
    acc.should == [1,2,3]
  end

  it "calls the method given in the constructor instead of #each" do
    each = mock('peach')
    each.should_receive(:peach)
    enumerator_class.new(each, :peach).each {|e| e }
  end

  it "calls the method given in the constructor until it's exhausted" do
    each = mock('each')
    each.should_receive(:each).and_yield(1).and_yield(2).and_yield(3)
    acc = []
    enumerator_class.new(each).each {|e| acc << e }
    acc.should == [1,2,3]
  end

  it "raises a NoMethodError if the object doesn't respond to #each" do
    lambda do
      enumerator_class.new(Object.new).each {|e| e }
    end.should raise_error(NoMethodError)
  end

  it "returns self if not given arguments and not given a block" do
    @enum_with_arguments.each.should equal(@enum_with_arguments)
  end

  it "returns the same value from receiver.each if block is given" do
    @enum_with_arguments.each {}.should equal(:method_returned)
  end

  it "passes given arguments at initialized to receiver.each" do
    @enum_with_arguments.each.to_a.should == [[:arg0, :arg1, :arg2]]
  end

  ruby_version_is "" ... "2.0" do
    it "does not require arguments" do
      enumerator_class.instance_method(:each).arity.should == 0
    end
  end

  ruby_version_is "2.0" do
    it "requires multiple arguments" do
      enumerator_class.instance_method(:each).arity.should < 0
    end

    it "appends given arguments to receiver.each" do
      @enum_with_arguments.each(:each0, :each1).to_a.should == [[:arg0, :arg1, :arg2, :each0, :each1]]
      @enum_with_arguments.each(:each2, :each3).to_a.should == [[:arg0, :arg1, :arg2, :each2, :each3]]
    end

    it "returns the same value from receiver.each if block and arguments are given" do
      @enum_with_arguments.each(:each1, :each2) {}.should equal(:method_returned)
    end

    it "returns new Enumerator if given arguments but not given a block" do
      ret = @enum_with_arguments.each 1
      ret.should be_an_instance_of(enumerator_class)
      ret.should_not equal(@enum_with_arguments)
    end
  end
end
