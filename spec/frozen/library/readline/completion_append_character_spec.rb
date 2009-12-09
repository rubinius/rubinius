require File.dirname(__FILE__) + '/../../spec_helper'

process_is_foreground do

  not_supported_on :ironruby do
    require 'readline'
    describe "Readline.completion_append_character" do
      it "returns not nil" do
        Readline.completion_append_character.should_not be_nil
      end
    end

    describe "Readline.completion_append_character=" do
      it "returns the first character of the passed string" do
        Readline.completion_append_character = "test"
        Readline.completion_append_character.should == "t"
      end
    end
  end
end
