require File.expand_path('../../spec_helper', __FILE__)

describe "A Flip2 node" do
  relates <<-ruby do
      x = if ((i % 4) == 0)..((i % 3) == 0) then
        i
      else
        nil
      end
    ruby

    # flip2
  end

  relates <<-ruby do
      if 1..2.a?(b) then
        nil
      end
    ruby

    # flip2 method
  end
end
