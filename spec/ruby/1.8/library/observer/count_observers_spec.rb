require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Observer#count_observers" do
  before(:each) do
    @observable = ObservableSpecs.new
    @observer = ObserverCallbackSpecs.new
  end

  it "counts the observers" do
    @observable.count_observers.should == 0
    @observable.add_observer(@observer)
    @observable.count_observers.should == 1
    @observable.add_observer(@observer)
    @observable.count_observers.should == 2
  end

end
