require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dxstr node" do
  relates <<-ruby do
      t = 5
      `touch \#{t}`
    ruby

    parse do
      [:block, [:lasgn, :t, [:lit, 5]], [:dxstr, "touch ", [:evstr, [:lvar, :t]]]]
    end

    # dxstr
  end
end
