require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using PrimitiveDeclaration transform" do
  relates <<-ruby do
      def m
        Ruby.primitive :prim
        raise PrimitiveFailure, "failed"
      end
    ruby

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
