require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using SendPrivately transform" do
  relates <<-ruby do
      def m
        c = 1
        Rubinius.privately {
          a.b c
        }
      end
    ruby

    compile :privately do |g|
      in_method :m do |d|
        d.push 1
        d.set_local 0
        d.pop

        d.push :self
        d.send :a, 0, true
        d.push_local 0
        d.send :b, 1, true
      end
    end
  end
end
