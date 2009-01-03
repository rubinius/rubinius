require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do
  require 'readline'

  not_supported_on :ironruby do
    describe "Readline.basic_word_break_characters" do
      it "needs to be reviewed for spec completeness"
    end

    describe "Readline.basic_word_break_characters=" do
      it "needs to be reviewed for spec completeness"
    end
  end
end
