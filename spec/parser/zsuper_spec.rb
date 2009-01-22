require File.dirname(__FILE__) + '/../spec_helper'

describe "A Zsuper node" do
  relates <<-ruby do
      def x
        super
      end
    ruby

    parse do
      [:defn, :x, [:args], [:scope, [:block, [:zsuper]]]]
    end

    # zsuper
  end
end
