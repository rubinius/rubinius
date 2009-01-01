require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do
  require 'readline'

  not_supported_on :ironruby do
    describe "Readline.completion_append_character" do
      it "needs to be reviewed for spec completeness"
    end

    describe "Readline.completion_append_character=" do
      it "needs to be reviewed for spec completeness"
    end
  end
end
