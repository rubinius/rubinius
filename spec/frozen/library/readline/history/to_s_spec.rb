require File.dirname(__FILE__) + '/../../../spec_helper'

process_is_foreground do
  require 'readline'

  not_supported_on :ironruby do
    describe "Readline::HISTORY.to_s" do
      it "returns 'HISTORY'" do
        Readline::HISTORY.to_s.should == "HISTORY"
      end
    end
  end
end
