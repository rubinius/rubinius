require File.expand_path('../../spec_helper', __FILE__)

describe "A Cvasgn node" do
  relates <<-ruby do
      def x
        @@blah = 1
      end
    ruby

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

    compile do |g|
      in_singleton_method :quiet_mode=, [:push, :self] do |d|
        d.push_scope
        d.push_literal :@@quiet_mode
        d.push_local 0
        d.send :class_variable_set, 2
      end
    end
  end
end
