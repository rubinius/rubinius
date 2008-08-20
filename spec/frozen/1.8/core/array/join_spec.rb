require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#join" do
  it "returns an empty string if the Array is empty" do
    a = []
    a.join.should == ''
  end

  it "returns a string formed by concatenating each element.to_s separated by separator without trailing separator" do
    obj = mock('foo')
    def obj.to_s() 'foo' end
    [1, 2, 3, 4, obj].join(' | ').should == '1 | 2 | 3 | 4 | foo'

# undef is not implemented -rue
#    obj = mock('o')
#    class << obj; undef :to_s; end
#    obj.should_receive(:method_missing).with(:to_s).and_return("o")
#    [1, obj].join(":").should == "1:o"
  end
  
  it "uses the same separator with nested arrays" do
    [1, [2, [3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
    [1, [2, ArraySpecs::MyArray[3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
  end

  it "uses $, as the default separator (which defaults to empty)" do
    [1, 2, 3].join.should == '123'
    old, $, = $,, '-'
    [1, 2, 3].join.should == '1-2-3'
    $, = old
  end
  
  it "tries to convert the passed seperator to a String using #to_str" do
    obj = mock('::')
    obj.should_receive(:to_str).and_return("::")
    [1, 2, 3, 4].join(obj).should == '1::2::3::4'
  end
  
  it "checks whether the passed seperator responds to #to_str" do
    obj = mock('.')
    obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_str).and_return(".")
    [1, 2].join(obj).should == "1.2"
  end

  it "does not process the separator if the array is empty" do
    a = []
    sep = Object.new
    a.join(sep).should == ""
  end

  it "handles recursive arrays" do
    x = []
    x << x
    x.join(":").should == '[...]'
    
    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    # representations when recursing from x
    # these are here to make it easier to understand what is happening
    y_rec = '9:[...]:8:9:[...]:8:[...]:7:7'
    x_rec = '1:[...]:2:' + y_rec + ':3'
    x.join(":").should == '1:' + x_rec + ':2:' + y_rec + ':3'

    x = ["one", "two"]
    x << x
    x.join('/').should == 'one/two/one/two/[...]'

    x << "three"
    x << "four"
    x.join('/').should == 'one/two/one/two/[...]/three/four/three/four'

    # nested and recursive
    x = [["one", "two"], ["three", "four"]]
    x << x
    x.join('/').should == 'one/two/three/four/one/two/three/four/[...]'
  end
end
