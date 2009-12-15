describe :array_join, :shared => true do
  it "returns an empty string if the Array is empty" do
    a = @object.new
    a.send(@method, ':').should == ''
  end

  ruby_version_is ""..."1.9" do
    it "returns a string formed by concatenating each element.to_s separated by separator without trailing separator" do
      obj = mock('foo')
      def obj.to_s() 'foo' end
      @object.new(1, 2, 3, 4, obj).send(@method, ' | ').should == '1 | 2 | 3 | 4 | foo'

      obj = mock('o')
      class << obj; undef :to_s; end
      obj.should_receive(:method_missing).with(:to_s).and_return("o")
      @object.new(1, obj).send(@method, ":").should == "1:o"
    end
  end

  ruby_version_is "1.9" do
    it "returns a string formed by concatenating each element.to_str separated by separator without trailing separator" do
      obj = mock('foo')
      def obj.to_s() 'foo' end
      @object.new(1, 2, 3, 4, obj).send(@method, ' | ').should == '1 | 2 | 3 | 4 | foo'

      obj = mock('o')
      obj.should_receive(:method_missing).with(:to_str).and_return("o")
      @object.new(1, obj).send(@method, ":").should == "1:o"
    end
  end

  it "raises a NoMethodError if an element does not respond to #to_s" do
    obj = mock('o')
    class << obj; undef :to_s; end
    lambda{ @object.new(1,obj).send(@method, ':') }.should raise_error(NoMethodError)
  end

  it "uses the same separator with nested arrays" do
    @object.new(1, @object.new(2, @object.new(3, 4), 5), 6).send(@method, ":").should == "1:2:3:4:5:6"
    @object.new(1, @object.new(2, ArraySpecs::MyArray[3, 4], 5), 6).send(@method, ":").should == "1:2:3:4:5:6"
  end

  it "tries to convert the passed separator to a String using #to_str" do
    obj = mock('::')
    obj.should_receive(:to_str).and_return("::")
    @object.new(1, 2, 3, 4).send(@method, obj).should == '1::2::3::4'
  end

  ruby_version_is ""..."1.9" do
    # Detail of joining recursive arrays is implementation dependent: [ruby-dev:37021]
    it "handles recursive arrays" do
      x = @object.new
      x << x
      x.send(@method, ':').should be_kind_of(String)

      x = @object.new("one", "two")
      x << x
      str = x.send(@method, '/')
      str.should include("one/two")

      x << "three"
      x << "four"
      str = x.send(@method, '/')
      str.should include("one/two")
      str.should include("three/four")

      # nested and recursive
      x = @object.new(@object.new("one", "two"), @object.new("three", "four"))
      x << x
      str = x.send(@method, '/')
      str.should include("one/two")
      str.should include("three/four")

      x = @object.new
      y = @object.new
      y << 9 << x << 8 << y << 7
      x << 1 << x << 2 << y << 3
      # representations when recursing from x
      # these are here to make it easier to understand what is happening
      str = x.send(@method, ':')
      str.should include('1')
      str.should include('2')
      str.should include('3')
    end
  end

  ruby_version_is "1.9" do
    it "raises an ArgumentError when the Array is recursive" do
      x = @object.new
      x << x
      lambda { x.send(@method, ':') }.should raise_error(ArgumentError)

      x = @object.new("one", "two")
      x << x
      lambda { x.send(@method, '/') }.should raise_error(ArgumentError)

      # nested and recursive
      x = @object.new(@object.new("one", "two"), @object.new("three", "four"))
      x << x
      lambda { x.send(@method, '/') }.should raise_error(ArgumentError)
    end
  end

  it "does not consider taint of either the array or the separator when the array is empty" do
    @object.new.send(@method, ":").tainted?.should == false
    @object.new.taint.send(@method, ":").tainted?.should == false
    @object.new.send(@method, ":".taint).tainted?.should == false
    @object.new.taint.send(@method, ":".taint).tainted?.should == false
  end

   # This doesn't work for Enumerable#join on 1.9. See bug #1732
  it "returns a string which would be infected with taint of the array, its elements or the separator when the array is not empty" do
    @object.new("a", "b").send(@method, ":").tainted?.should == false
    @object.new("a", "b").send(@method, ":".taint).tainted?.should == true
    @object.new("a", "b").taint.send(@method, ":").tainted?.should == true
    @object.new("a", "b").taint.send(@method, ":".taint).tainted?.should == true
    @object.new("a", "b".taint).send(@method, ":").tainted?.should == true
    @object.new("a", "b".taint).send(@method, ":".taint).tainted?.should == true
    @object.new("a", "b".taint).taint.send(@method, ":").tainted?.should == true
    @object.new("a", "b".taint).taint.send(@method, ":".taint).tainted?.should == true
    @object.new("a".taint, "b").send(@method, ":").tainted?.should == true
    @object.new("a".taint, "b").send(@method, ":".taint).tainted?.should == true
    @object.new("a".taint, "b").taint.send(@method, ":").tainted?.should == true
    @object.new("a".taint, "b").taint.send(@method, ":".taint).tainted?.should == true
    @object.new("a".taint, "b".taint).send(@method, ":").tainted?.should == true
    @object.new("a".taint, "b".taint).send(@method, ":".taint).tainted?.should == true
    @object.new("a".taint, "b".taint).taint.send(@method, ":").tainted?.should == true
    @object.new("a".taint, "b".taint).taint.send(@method, ":".taint).tainted?.should == true
  end

  ruby_version_is '1.9' do
    it "does not consider untrustworthiness of either the array or the separator when the array is empty" do
      @object.new.send(@method, ":").untrusted?.should == false
      @object.new.untrust.send(@method, ":").untrusted?.should == false
      @object.new.send(@method, ":".untrust).untrusted?.should == false
      @object.new.untrust.send(@method, ":".untrust).untrusted?.should == false
    end

    # This doesn't work for Enumerable#join on 1.9. See bug #1732
    it "returns a string which would be infected with untrustworthiness of the array, its elements or the separator when the array is not empty" do
      @object.new("a", "b").send(@method, ":").untrusted?.should == false
      @object.new("a", "b").send(@method, ":".untrust).untrusted?.should == true
      @object.new("a", "b").untrust.send(@method, ":").untrusted?.should == true
      @object.new("a", "b").untrust.send(@method, ":".untrust).untrusted?.should == true
      @object.new("a", "b".untrust).send(@method, ":").untrusted?.should == true
      @object.new("a", "b".untrust).send(@method, ":".untrust).untrusted?.should == true
      @object.new("a", "b".untrust).untrust.send(@method, ":").untrusted?.should == true
      @object.new("a", "b".untrust).untrust.send(@method, ":".untrust).untrusted?.should == true
      @object.new("a".untrust, "b").send(@method, ":").untrusted?.should == true
      @object.new("a".untrust, "b").send(@method, ":".untrust).untrusted?.should == true
      @object.new("a".untrust, "b").untrust.send(@method, ":").untrusted?.should == true
      @object.new("a".untrust, "b").untrust.send(@method, ":".untrust).untrusted?.should == true
      @object.new("a".untrust, "b".untrust).send(@method, ":").untrusted?.should == true
      @object.new("a".untrust, "b".untrust).send(@method, ":".untrust).untrusted?.should == true
      @object.new("a".untrust, "b".untrust).untrust.send(@method, ":").untrusted?.should == true
      @object.new("a".untrust, "b".untrust).untrust.send(@method, ":".untrust).untrusted?.should == true
    end
  end
end
