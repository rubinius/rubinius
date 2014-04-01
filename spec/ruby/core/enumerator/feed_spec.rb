require File.expand_path('../../../spec_helper', __FILE__)

describe "Enumerator#feed" do
  before :each do
    # An object to keep track of the last yield return value
    @o = Object.new
    class << @o
      attr_reader :last_yield

      def each
        while true
          @last_yield = yield
        end
      end
    end
  end

  it "changes the next return value of yield" do
    e = @o.to_enum
    e.next
    e.feed "bar"
    e.next
    @o.last_yield.should == "bar"
  end

  it "clears the feed value after being yielded" do
    e = @o.to_enum
    e.next
    e.feed "bar"
    e.next
    e.next
    @o.last_yield.should be_nil
  end

  it "raises a TypeError when called repeatedly" do
    e = [].to_enum
    e.feed 0
    lambda { e.feed 0 }.should raise_error(TypeError)
    lambda { e.feed 0 }.should raise_error(TypeError)
  end

  it "returns nil" do
    e = [].to_enum
    e.feed(5).should be_nil
  end

  it "resets if rewind is called" do
    e = @o.to_enum
    e.next
    e.feed "bar"
    e.rewind
    e.next
    @o.last_yield.should be_nil
  end

  it "matches the ruby-doc example" do
    e = [1,2,3].map
    e.next
    e.feed "a"
    e.next
    e.feed "b"
    e.next
    e.feed "c"
    @x = nil
    begin
        e.next
    rescue StopIteration
        @x = $!.result
    end
    @x.should == ["a", "b", "c"]
  end
end
