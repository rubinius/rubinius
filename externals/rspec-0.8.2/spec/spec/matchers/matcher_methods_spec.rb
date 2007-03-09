require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Matchers
    
    context "Spec::Matchers should support" do
      specify "be_true" do
        be_true.should be_an_instance_of(Be)
      end
      specify "be_false" do
        be_false.should be_an_instance_of(Be)
      end
      specify "be_nil" do
        be_nil.should be_an_instance_of(Be)
      end
      specify "be_arbitrary_predicate" do
        be_arbitrary_predicate.should be_an_instance_of(Be)
      end
      specify "be_close" do
        be_close(1,2).should be_an_instance_of(BeClose)
      end
      specify "change" do
        change("target", :message).should be_an_instance_of(Change)
      end
      specify "eql" do
        eql(:expected).should be_an_instance_of(Eql)
      end
      specify "equal" do
        equal(:expected).should be_an_instance_of(Equal)
      end
      specify "have_x > has_x?" do
        have_key(:key).should be_an_instance_of(Has)
      end
      specify "have" do
        have(0).should be_an_instance_of(Have)
      end
      specify "have_exactly" do
        have_exactly(0).should be_an_instance_of(Have)
      end
      specify "have_at_least" do
        have_at_least(0).should be_an_instance_of(Have)
      end
      specify "have_at_most" do
        have_at_most(0).should be_an_instance_of(Have)
      end
      specify "include" do
        include(:value).should be_an_instance_of(Include)
      end
      specify "match" do
        match(:value).should be_an_instance_of(Match)
      end
      specify "raise_error" do
        raise_error.should be_an_instance_of(RaiseError)
        raise_error(NoMethodError).should be_an_instance_of(RaiseError)
        raise_error(NoMethodError, "message").should be_an_instance_of(RaiseError)
      end
      specify "satisfy" do
        satisfy{}.should be_an_instance_of(Satisfy)
      end
      specify "throw_symbol" do
        throw_symbol.should be_an_instance_of(ThrowSymbol)
        throw_symbol(:sym).should be_an_instance_of(ThrowSymbol)
      end
      specify "respond_to" do
        respond_to(:sym).should be_an_instance_of(RespondTo)
      end
      
    end
    
    context "Spec::Matchers should convert be_xyz to..." do
      specify "sym passed to Be" do
        Be.should_receive(:new).with(:be_whatever)
        be_whatever
      end
    end
    
    context "Spec::Matchers should convert have_xyz to..." do
      specify "string passed to Has" do
        Has.should_receive(:new).with(:have_whatever)
        have_whatever
      end
    end

  end
end
