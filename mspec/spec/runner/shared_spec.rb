require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/runner/shared'

describe Object, "#it_behaves_like" do
  before :each do
    @recv = Object.new
    def @recv.before(what)
      yield
    end
    @recv.stub!(:it_should_behave_like)
  end

  it "creates @method set to the name of the aliased method" do
    @recv.it_behaves_like "something", :some_method
    @recv.instance_variable_get(:@method).should == :some_method
  end

  it "creates @object if the passed object is not nil" do
    @recv.it_behaves_like "something", :some_method, :some_object
    @recv.instance_variable_get(:@object).should == :some_object
  end

  it "sends :it_should_behave_like" do
    @recv.should_receive(:it_should_behave_like)
    @recv.it_behaves_like "something", :some_method
  end
end
