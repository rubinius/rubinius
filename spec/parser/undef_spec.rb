require File.dirname(__FILE__) + '/../spec_helper'

describe "An Undef node" do
  relates "undef :x" do
    parse do
      [:undef, [:lit, :x]]
    end

    # undef
  end

  relates "undef :x, :y" do
    parse do
      [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]]
    end

    # undef 2
  end

  relates "undef :x, :y, :z" do
    parse do
      [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]]
    end

    # undef 3
  end

  relates <<-ruby do
      f1
      undef :x
    ruby

    parse do
      [:block, [:call, nil, :f1, [:arglist]], [:undef, [:lit, :x]]]
    end

    # undef block 1
  end

  relates <<-ruby do
      f1
      undef :x, :y
    ruby

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]]]
    end

    # undef block 2
  end

  relates <<-ruby do
      undef :x, :y, :z
      f2
    ruby

    parse do
      [:block,
       [:undef, [:lit, :x]],
       [:undef, [:lit, :y]],
       [:undef, [:lit, :z]],
       [:call, nil, :f2, [:arglist]]]
    end

    # undef block 3 post
  end

  relates <<-ruby do
      f1
      undef :x, :y, :z
    ruby

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]]]
    end

    # undef block 3
  end

  relates <<-ruby do
      f1
      undef :x, :y, :z
      f2
    ruby

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]],
       [:call, nil, :f2, [:arglist]]]
    end

    # undef block wtf
  end

end
