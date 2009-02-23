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

    compile do |g|
      in_method :x do |d|
        d.push_scope
        d.push_literal :@@blah
        d.push 1
        d.send :class_variable_set, 2
      end
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

    compile do |g|
      g.push :self
      in_method :quiet_mode=, :singleton do |d|
        d.push_scope
        d.push_literal :@@quiet_mode
        d.push_local 0
        d.send :class_variable_set, 2
      end
    end
  end
end
