require File.dirname(__FILE__) + '/../spec_helper'

describe "A Return node" do
  relates "return" do
    parse do
      [:return]
    end

    # return 0
  end

  relates "return 1" do
    parse do
      [:return, [:lit, 1]]
    end

    # return 1
  end

  relates "return *1" do
    parse do
      [:return, [:svalue, [:splat, [:lit, 1]]]]
    end

    # return 1 splatted
  end

  relates "return 1, 2, 3" do
    parse do
      [:return, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]
    end

    # return n
  end

  relates "return [*[1]]" do
    parse do
      [:return, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat return array
  end

  relates "return *[1]" do
    parse do
      [:return, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat return
  end
end
