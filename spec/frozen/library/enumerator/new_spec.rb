require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  require 'enumerator'
  require File.dirname(__FILE__) + '/../../shared/enumerator/new'

  describe "Enumerator.new" do
    it "requires an argument" do
      lambda {enumerator_class.new}.should raise_error(ArgumentError)
    end

    it_behaves_like(:enum_new, :new)
  end
end
