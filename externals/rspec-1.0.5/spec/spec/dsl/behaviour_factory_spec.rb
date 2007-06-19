require File.dirname(__FILE__) + '/../../spec_helper'

module Spec
  module DSL
    describe BehaviourFactory do
      it "should create a Spec::DSL::Behaviour by default" do
        Spec::DSL::BehaviourFactory.create("behaviour") {
        }.should be_an_instance_of(Spec::DSL::Behaviour)
      end

      it "should create a Spec::DSL::Behaviour when :behaviour_type => :default" do
        Spec::DSL::BehaviourFactory.create("behaviour", :behaviour_type => :default) {
        }.should be_an_instance_of(Spec::DSL::Behaviour)
      end

      it "should create specified type when :behaviour_type => :something_other_than_default" do
        behaviour_class = Class.new do
          def initialize(*args, &block); end
        end
        Spec::DSL::BehaviourFactory.add_behaviour_class(:something_other_than_default, behaviour_class)
        Spec::DSL::BehaviourFactory.create("behaviour", :behaviour_type => :something_other_than_default) {
        }.should be_an_instance_of(behaviour_class)
      end
      
      after(:each) do
        Spec::DSL::BehaviourFactory.remove_behaviour_class(:something_other_than_default)
      end
    end
  end
end
