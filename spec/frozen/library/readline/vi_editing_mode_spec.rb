require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do
  require 'readline'

  not_supported_on :ironruby do
    describe "Readline.vi_editing_mode" do
      it "needs to be reviewed for spec completeness"
    end
  end
end
