require File.dirname(__FILE__) + '/../../../spec_helper'

process_is_foreground do
  with_feature :readline do
    require 'readline'

    describe "Readline::HISTORY" do
      it "is extended with the Enumerable module" do
        Readline::HISTORY.should be_kind_of(Enumerable)
      end
    end
  end
end
