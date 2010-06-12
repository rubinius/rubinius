require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

ruby_version_is "1.8.8" do
  describe "Matrix#each" do
    before :all do
      @m = Matrix[ [1, 2, 3], [4, 5, 6] ]
      @result = (1..6).to_a
    end

    it "returns an Enumerator when called without a block" do
      enum = @m.each
      enum.should be_an_instance_of(enumerator_class)
      enum.to_a.should == @result
    end

    it "returns self" do
      @m.each{}.should equal(@m)
    end

    it "yields the elements starting with the those of the first row" do
      a = []
      @m.each {|x| a << x}
      a.should ==  @result
    end
  end
end