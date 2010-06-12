require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using SendFastCoerceTo transform" do
  relates <<-ruby do
      a = b
      Type.coerce_to a, Fixnum, :to_int
    ruby

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.set_local 0
      g.pop

      g.push_const :Type
      g.push_local 0
      g.push_const :Fixnum
      g.push_literal :to_int
      g.send :coerce_to, 3, false
    end

    compile :fast_coerce do |g|
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.set_local 0
      g.pop

      g.push_local 0
      g.dup
      g.send :__fixnum__, 0
      g.git done

      g.pop
      g.push_const :Type
      g.push_local 0
      g.push_const :Fixnum
      g.push_literal :to_int
      g.send :coerce_to, 3, false

      done.set!
    end
  end

  relates <<-ruby do
      Type.coerce_to a, Fixnum, :to_int
    ruby

    compile do |g|
      g.push_const :Type
      g.push :self
      g.send :a, 0, true
      g.push_const :Fixnum
      g.push_literal :to_int
      g.send :coerce_to, 3, false
    end

    compile :fast_coerce do |g|
      g.push_const :Type
      g.push :self
      g.send :a, 0, true
      g.push_const :Fixnum
      g.push_literal :to_int
      g.send :coerce_to, 3, false
    end
  end

  relates <<-ruby do
      a = b
      Type.coerce_to a, String, :to_str
    ruby

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.set_local 0
      g.pop

      g.push_const :Type
      g.push_local 0
      g.push_const :String
      g.push_literal :to_str
      g.send :coerce_to, 3, false
    end

    compile :fast_coerce do |g|
      g.push :self
      g.send :b, 0, true
      g.set_local 0
      g.pop

      g.push_const :Type
      g.push_local 0
      g.push_const :String
      g.push_literal :to_str
      g.send :coerce_to, 3, false
    end
  end
end
