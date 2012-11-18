# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

with_feature :encoding do
  describe "Rubinius::Character#space?" do
    it "returns true for a whitespace characters" do
      [ [new_character("\n"), true],
        [new_character("\r"), true],
        [new_character("\f"), true],
        [new_character("\t"), true],
        [new_character("\v"), true],
        [new_character(" "),  true],
      ].should be_computed_by(:space?)
    end

    it "returns false for a non-whitespace character" do
      char = new_character "a"
      char.space?.should be_false
    end
  end
end
