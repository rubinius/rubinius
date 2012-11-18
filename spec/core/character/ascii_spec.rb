# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#ascii?" do
    it "returns true for an ASCII character" do
      char = new_character "a"
      char.ascii?.should be_true
    end

    it "returns false for a non-ASCII character" do
      char = new_character "あ"
      char.ascii?.should be_false
    end
  end
end
