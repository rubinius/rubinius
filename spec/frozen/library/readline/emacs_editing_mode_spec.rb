require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do

  not_supported_on :ironruby do
    require 'readline'
    describe "Readline.emacs_editing_mode" do
      it "returns nil" do
        Readline.emacs_editing_mode.should be_nil
      end
    end
  end
end
