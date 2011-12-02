require File.expand_path('../../../spec_helper', __FILE__)

describe "Enumerable#one?" do
  ruby_version_is '1.8.7' do
    describe "when passed a block" do
      it "returns true if block returns true once" do
        [:a, :b, :c].one? { |s| s == :a }.should be_true
      end

      it "returns false if the block returns true more than once" do
        [:a, :b, :c].one? { |s| s == :a || s == :b }.should be_false
      end

      it "returns false if the block only returns false" do
        [:a, :b, :c].one? { |s| s == :d }.should be_false
      end
    end

    describe "when not passed a block" do
      it "returns true if only one element evaluates to true" do
        [false, nil, true].one?.should be_true
      end

      it "returns false if two elements evaluate to true" do
        [false, :value, nil, true].one?.should be_false
      end

      it "returns false if all elements evaluate to false" do
        [false, nil, false].one?.should be_false
      end
    end
  end
end
