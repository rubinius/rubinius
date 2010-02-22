require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Array#to_s" do
  ruby_version_is "" .. "1.9" do
    it "is equivalent to #join without a separator string" do
      old = $,
      begin
        a = [1, 2, 3, 4]
        a.to_s.should == a.join
        $, = '-'
        a.to_s.should == a.join
      ensure
        $, = old
      end
    end

    it "returns '[...]' for an empty array embedded in itself" do
      ArraySpecs.empty_recursive_array.to_s.should == "[...]"
    end

    ruby_bug "[ruby-dev:37019]", "1.8.6.319" do
      it "returns '[...]' for the embedded array in a recursive array" do
        a = [1, 2, 3]; a << a
        a.to_s.should == "123[...]"
      end
    end
  end

  ruby_version_is "1.9" do
    it "is equivalent to Array#inspect" do
      a = [1, 2, 3, 4]
      a.to_s.should == a.inspect
    end

    it "is equivalent to Array#inspect in a recursive array" do
      a = [1, 2, 3, 4]; a << a
      a.to_s.should == a.inspect

      a = ArraySpecs.empty_recursive_array
      a.to_s.should == "[[...]]"
      a.inspect == "[[...]]"
    end
  end
end
