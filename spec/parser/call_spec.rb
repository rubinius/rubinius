require File.dirname(__FILE__) + '/../spec_helper'

describe "A Call node" do
  relates "self.method" do
    parse do
      [:call, [:self], :method, [:arglist]]
    end

    # call
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end
    ruby

    parse do
      [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]
    end

    # begin
  end

  relates "a.b(&c)" do
    parse do
      [:call,
       [:call, nil, :a, [:arglist]],
       :b,
       [:arglist, [:block_pass, [:call, nil, :c, [:arglist]]]]]
    end

    # block_pass_call_0
  end

  relates "a.b(4, &c)" do
    parse do
      [:call,
       [:call, nil, :a, [:arglist]],
       :b,
       [:arglist, [:lit, 4], [:block_pass, [:call, nil, :c, [:arglist]]]]]
    end

    # block_pass_call_1
  end

  relates "a.b(1, 2, 3, &c)" do
    parse do
      [:call,
       [:call, nil, :a, [:arglist]],
       :b,
       [:arglist,
        [:lit, 1],
        [:lit, 2],
        [:lit, 3],
        [:block_pass, [:call, nil, :c, [:arglist]]]]]
    end

    # block pass call n
  end

  relates "a(&b)" do
    parse do
      [:call,
       nil,
       :a,
       [:arglist, [:block_pass, [:call, nil, :b, [:arglist]]]]]
    end

    # block pass fcall 0
  end

  relates "a(4, &b)" do
    parse do
      [:call,
       nil,
       :a,
       [:arglist, [:lit, 4], [:block_pass, [:call, nil, :b, [:arglist]]]]]
    end

    # block pass fcall 1
  end

  relates "a(1, 2, 3, &b)" do
    parse do
      [:call,
       nil,
       :a,
       [:arglist,
        [:lit, 1],
        [:lit, 2],
        [:lit, 3],
        [:block_pass, [:call, nil, :b, [:arglist]]]]]
    end

    # block pass fcall n
  end

  relates "define_attr_method(:x, :sequence_name, &Proc.new { |*args| nil })" do
    parse do
      [:call,
       nil,
       :define_attr_method,
       [:arglist,
        [:lit, :x],
        [:lit, :sequence_name],
        [:block_pass,
         [:iter,
          [:call, [:const, :Proc], :new, [:arglist]],
          [:masgn, [:array, [:splat, [:lasgn, :args]]]],
          [:nil]]]]]
    end

    # block pass omgwtf
  end

  relates "r.read_body(dest, &block)" do
    parse do
      [:call,
       [:call, nil, :r, [:arglist]],
       :read_body,
       [:arglist,
        [:call, nil, :dest, [:arglist]],
        [:block_pass, [:call, nil, :block, [:arglist]]]]]
    end

    # block pass thingy
  end

  relates "o.m(:a => 1, :b => 2)" do
    parse do
      [:call,
       [:call, nil, :o, [:arglist]],
       :m,
       [:arglist, [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]]
    end

    # call arglist hash
  end

  relates "o.m(42, :a => 1, :b => 2)" do
    parse do
      [:call,
       [:call, nil, :o, [:arglist]],
       :m,
       [:arglist,
        [:lit, 42],
        [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]]
    end

    # call arglist norm hash
  end

  relates "o.m(42, :a => 1, :b => 2, *c)" do
    parse do
      [:call,
       [:call, nil, :o, [:arglist]],
       :m,
       [:arglist,
        [:lit, 42],
        [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]],
        [:splat, [:call, nil, :c, [:arglist]]]]]
    end

    # call arglist norm hash
  end

  relates "a (1,2,3)" do
    parse do
      [:call, nil, :a, [:arglist, [:lit, 1], [:lit, 2], [:lit, 3]]]
    end

    # call arglist space
  end

  relates "o.puts(42)" do
    parse do
      [:call, [:call, nil, :o, [:arglist]], :puts, [:arglist, [:lit, 42]]]
    end

    # call arglist
  end

  relates "1.b(c)" do
    parse do
      [:call, [:lit, 1], :b, [:arglist, [:call, nil, :c, [:arglist]]]]
    end

    # call command
  end

  relates "(v = (1 + 1)).zero?" do
    parse do
      [:call,
       [:lasgn, :v, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       :zero?,
       [:arglist]]
    end

    # call expr
  end

  relates "-2**31" do
    parse do
      [:call,
       [:call, [:lit, 2], :**, [:arglist, [:lit, 31]]],
       :-@,
       [:arglist]]
    end

    # call unary neg
  end

  relates "a[]" do
    parse do
      [:call, [:call, nil, :a, [:arglist]], :[], [:arglist]]
    end

    # call index no args
  end

  relates "m(:a => 1, :b => 2)" do
    parse do
      [:call,
       nil,
       :m,
       [:arglist, [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]]
    end

    # fcall arglist hash
  end

  relates "m(42, :a => 1, :b => 2)" do
    parse do
      [:call,
       nil,
       :m,
       [:arglist, [:lit, 42], [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]]
    end

    # fcall arglist norm hash
  end

  relates "m(42, :a => 1, :b => 2, *c)" do
    parse do
      [:call,
       nil,
       :m,
       [:arglist,
        [:lit, 42],
        [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]],
        [:splat, [:call, nil, :c, [:arglist]]]]]
    end

    # fcall arglist norm hash splat
  end

  relates "m(42)" do
    parse do
      [:call, nil, :m, [:arglist, [:lit, 42]]]
    end

    # fcall arglist
  end

  relates "a(:b) { :c }" do
    parse do
      [:iter, [:call, nil, :a, [:arglist, [:lit, :b]]], nil, [:lit, :c]]
    end

    # fcall block
  end

  relates "a [42]" do
    parse do
      [:call, nil, :a, [:arglist, [:array, [:lit, 42]]]]
    end

    # fcall index space
  end

  relates "42 if block_given?" do
    parse do
      [:if, [:call, nil, :block_given?, [:arglist]], [:lit, 42], nil]
    end

    # fcall keyword
  end

  relates "method" do
    parse do
      [:call, nil, :method, [:arglist]]
    end

    # vcall
  end

  relates <<-ruby do
      a << begin
             b
           rescue
             c
           end
    ruby

    parse do
      [:call,
       [:call, nil, :a, [:arglist]],
       :<<,
       [:arglist,
        [:rescue,
         [:call, nil, :b, [:arglist]],
         [:resbody, [:array], [:call, nil, :c, [:arglist]]]]]]
    end

    # structure remove begin 1
  end

  relates "meth([*[1]])" do
    parse do
      [:call,
        nil,
        :meth,
        [:arglist, [:array, [:splat, [:array, [:lit, 1]]]]]]
    end

    # splat fcall array
  end

  relates "meth(*[1])" do
    parse do
      [:call, nil, :meth, [:arglist, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat fcall
  end
end
