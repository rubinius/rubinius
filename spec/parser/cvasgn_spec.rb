require File.dirname(__FILE__) + '/../spec_helper'

describe "A Cvasgn node" do
  relates <<-ruby do
      def x
        @@blah = 1
      end
    ruby

    parse do
      [:defn,
       :x,
       [:args],
       [:scope, [:block, [:cvasgn, :@@blah, [:lit, 1]]]]]
    end

  end

  relates <<-ruby do
      def self.quiet_mode=(boolean)
        @@quiet_mode = boolean
      end
    ruby

    parse do
      [:defs,
       [:self],
       :quiet_mode=,
       [:args, :boolean],
       [:scope, [:block, [:cvasgn, :@@quiet_mode, [:lvar, :boolean]]]]]
    end

    # cvasgn cls method
  end
end
