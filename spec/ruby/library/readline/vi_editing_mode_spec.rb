require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do
  with_feature :readline do
    require 'readline'

    describe "Readline.vi_editing_mode" do
      it "returns nil" do
        Readline.vi_editing_mode.should be_nil
      end
    end
  end
end
