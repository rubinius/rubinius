require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using SendPrivately transform" do
  relates <<-ruby do
      def m
        Rubinius.privately {
          a.b c
        }
      end
    ruby

    compile :privately do |g|
      in_method :m do |d|
        d.push :self
        d.send :a, 0, true
        d.push :self
        d.send :c, 0, true
        d.send :b, 1, true
      end
    end
  end
end
