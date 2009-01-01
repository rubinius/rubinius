require File.dirname(__FILE__) + '/../../../spec_helper'

process_is_foreground do
  require 'readline'

  not_supported_on :ironruby do
    describe "Readline::HISTORY" do
      it "is extended with the Enumerable module" do
        Readline::HISTORY.should be_kind_of(Enumerable)
      end
    end
  end
end
