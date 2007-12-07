require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#join" do
  it "returns a string formed by concatenating each element.to_s separated by separator without trailing separator" do
    obj = Object.new
    def obj.to_s() 'foo' end
    [1, 2, 3, 4, obj].join(' | ').should == '1 | 2 | 3 | 4 | foo'

# undef is not implemented -rue
#    obj = Object.new
#    class << obj; undef :to_s; end
#    obj.should_receive(:method_missing).with(:to_s).and_return("o")
#    [1, obj].join(":").should == "1:o"
  end
  
  it "uses the same separator with nested arrays" do
    [1, [2, [3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
    [1, [2, MyArray[3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
  end

  it "uses $, as the default separator (which defaults to empty)" do
    [1, 2, 3].join.should == '123'
    old, $, = $,, '-'
    [1, 2, 3].join.should == '1-2-3'
    $, = old
  end
  
  it "calls to_str on its separator argument" do
    obj = Object.new
    def obj.to_str() '::' end    
    [1, 2, 3, 4].join(obj).should == '1::2::3::4'
    
    obj = Object.new
    obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_str).and_return(".")
    [1, 2].join(obj).should == "1.2"
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
  end
end
