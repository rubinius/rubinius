require File.dirname(__FILE__) + '/../spec_helper'

describe "A Xstr node" do
  relates "`touch 5`" do
    compile do |g|
      g.push :self
      g.push_literal "touch 5"
      g.string_dup
      g.send :"`", 1, true
    end
  end
end
