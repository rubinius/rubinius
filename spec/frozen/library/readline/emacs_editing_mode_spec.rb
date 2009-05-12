require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do

  not_supported_on :ironruby do
    require 'readline'
    describe "Readline.emacs_editing_mode" do
      it "needs to be reviewed for spec completeness"
    end
  end
end
