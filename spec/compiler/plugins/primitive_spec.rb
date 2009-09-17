require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using PrimitiveDeclaration plugin" do
  relates <<-ruby do
      def m
        Ruby.primitive :prim
        raise PrimitiveFailure, "failed"
      end
    ruby

    parse do
      [:defn,
       :m,
       [:args],
       [:scope,
        [:block,
         [:call, [:const, :Ruby], :primitive, [:arglist, [:lit, :prim]]],
         [:call,
          nil,
          :raise,
          [:arglist, [:const, :PrimitiveFailure], [:str, "failed"]]]]]]
    end

    compile do |g|
      in_method :m do |d|
        d.push_const :Ruby
        d.push_literal :prim
        d.send :primitive, 1, false
        d.pop
        d.push :self
        d.push_const :PrimitiveFailure
        d.push_literal "failed"
        d.string_dup
        d.send :raise, 2, true
      end
    end

    compile :primitive do |g|
      in_method :m do |d|
        d.push :self
        d.push_const :PrimitiveFailure
        d.push_literal "failed"
        d.string_dup
        d.send :raise, 2, true
      end
    end
  end
end
