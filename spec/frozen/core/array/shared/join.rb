describe :array_join, :shared => true do
  it "returns an empty string if the Array is empty" do
    a = []
    a.send(@method, ':').should == ''
  end

  it "returns a string formed by concatenating each element.to_s separated by separator without trailing separator" do
    obj = mock('foo')
    def obj.to_s() 'foo' end
    [1, 2, 3, 4, obj].send(@method, ' | ').should == '1 | 2 | 3 | 4 | foo'

    obj = mock('o')
    class << obj; undef :to_s; end
    obj.should_receive(:method_missing).with(:to_s).and_return("o")
    [1, obj].send(@method, ":").should == "1:o"
  end

  it "raises a NoMethodError if an element does not respond to #to_s" do
    obj = mock('o')
    class << obj; undef :to_s; end
    lambda{ [1,obj].send(@method, ':') }.should raise_error(NoMethodError)
  end

  it "uses the same separator with nested arrays" do
    [1, [2, [3, 4], 5], 6].send(@method, ":").should == "1:2:3:4:5:6"
    [1, [2, ArraySpecs::MyArray[3, 4], 5], 6].send(@method, ":").should == "1:2:3:4:5:6"
  end

  it "tries to convert the passed separator to a String using #to_str" do
    obj = mock('::')
    obj.should_receive(:to_str).and_return("::")
    [1, 2, 3, 4].send(@method, obj).should == '1::2::3::4'
  end

  # detail of joining recursive arrays is implementation depended. [ruby-dev:37021]
  it "handles recursive arrays" do
    x = []
    x << x
    x.send(@method, ':').should be_kind_of(String)

    x = ["one", "two"]
    x << x
    str = x.send(@method, '/')
    str.should include("one/two")

    x << "three"
    x << "four"
    str = x.send(@method, '/')
    str.should include("one/two")
    str.should include("three/four")

    # nested and recursive
    x = [["one", "two"], ["three", "four"]]
    x << x
    str = x.send(@method, '/')
    str.should include("one/two")
    str.should include("three/four")

    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    # representations when recursing from x
    # these are here to make it easier to understand what is happening
    str = x.send(@method, ':')
    str.should include('1')
    str.should include('2')
    str.should include('3')
  end

  it "does not consider taint of either the array or the separator when the array is empty" do
    [].send(@method, ":").tainted?.should == false
    [].taint.send(@method, ":").tainted?.should == false
    [].send(@method, ":".taint).tainted?.should == false
    [].taint.send(@method, ":".taint).tainted?.should == false
  end

  it "returns a string which would be infected with taint of the array, its elements or the separator when the array is not empty" do
    ["a", "b"].send(@method, ":").tainted?.should == false
    ["a", "b"].send(@method, ":".taint).tainted?.should == true
    ["a", "b"].taint.send(@method, ":").tainted?.should == true
    ["a", "b"].taint.send(@method, ":".taint).tainted?.should == true
    ["a", "b".taint].send(@method, ":").tainted?.should == true
    ["a", "b".taint].send(@method, ":".taint).tainted?.should == true
    ["a", "b".taint].taint.send(@method, ":").tainted?.should == true
    ["a", "b".taint].taint.send(@method, ":".taint).tainted?.should == true
    ["a".taint, "b"].send(@method, ":").tainted?.should == true
    ["a".taint, "b"].send(@method, ":".taint).tainted?.should == true
    ["a".taint, "b"].taint.send(@method, ":").tainted?.should == true
    ["a".taint, "b"].taint.send(@method, ":".taint).tainted?.should == true
    ["a".taint, "b".taint].send(@method, ":").tainted?.should == true
    ["a".taint, "b".taint].send(@method, ":".taint).tainted?.should == true
    ["a".taint, "b".taint].taint.send(@method, ":").tainted?.should == true
    ["a".taint, "b".taint].taint.send(@method, ":".taint).tainted?.should == true
  end

  ruby_version_is '1.9' do
    it "does not consider untrustworthiness of either the array or the separator when the array is empty" do
      [].send(@method, ":").untrusted?.should == false
      [].untrust.send(@method, ":").untrusted?.should == false
      [].send(@method, ":".untrust).untrusted?.should == false
      [].untrust.send(@method, ":".untrust).untrusted?.should == false
    end

    it "returns a string which would be infected with untrustworthiness of the array, its elements or the separator when the array is not empty" do
      ["a", "b"].send(@method, ":").untrusted?.should == false
      ["a", "b"].send(@method, ":".untrust).untrusted?.should == true
      ["a", "b"].untrust.send(@method, ":").untrusted?.should == true
      ["a", "b"].untrust.send(@method, ":".untrust).untrusted?.should == true
      ["a", "b".untrust].send(@method, ":").untrusted?.should == true
      ["a", "b".untrust].send(@method, ":".untrust).untrusted?.should == true
      ["a", "b".untrust].untrust.send(@method, ":").untrusted?.should == true
      ["a", "b".untrust].untrust.send(@method, ":".untrust).untrusted?.should == true
      ["a".untrust, "b"].send(@method, ":").untrusted?.should == true
      ["a".untrust, "b"].send(@method, ":".untrust).untrusted?.should == true
      ["a".untrust, "b"].untrust.send(@method, ":").untrusted?.should == true
      ["a".untrust, "b"].untrust.send(@method, ":".untrust).untrusted?.should == true
      ["a".untrust, "b".untrust].send(@method, ":").untrusted?.should == true
      ["a".untrust, "b".untrust].send(@method, ":".untrust).untrusted?.should == true
      ["a".untrust, "b".untrust].untrust.send(@method, ":").untrusted?.should == true
      ["a".untrust, "b".untrust].untrust.send(@method, ":".untrust).untrusted?.should == true
    end
  end
end
