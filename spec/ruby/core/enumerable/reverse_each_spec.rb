require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is '1.8.7' do
  describe "Enumerable#reverse_each" do

    it "traverses enum in reverse order and pass each element to block" do
      a=[]
      EnumerableSpecs::Numerous.new.reverse_each { |i| a << i }
      a.should == [4, 1, 6, 3, 5, 2]
    end

    it 'returns an Enumerator if no block given' do
      enum = EnumerableSpecs::Numerous.new.reverse_each
      enum.should be_an_instance_of(enumerator_class)
      enum.to_a.should == [4, 1, 6, 3, 5, 2]
    end
  end
end
