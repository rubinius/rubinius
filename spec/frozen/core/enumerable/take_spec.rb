require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/take'

describe "Enumerable#take" do
  ruby_version_is '1.8.7' do
    it "requires an argument" do
      lambda{ EnumerableSpecs::Numerous.new.take}.should raise_error(ArgumentError)
    end
    
    describe "when passed an argument" do
      it_behaves_like :enumerable_take, :take
    end
  end
end