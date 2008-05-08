require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do

  it "compiles 'blah'" do
    gen [:vcall, :blah] do |g|
      g.push :self
      g.send :blah, 0, true
    end
  end

  it "compiles 'blah()'" do
    gen [:fcall, :blah] do |g|
      g.push :self
      g.send :blah, 0, true 
    end
  end

  it "compiles 'blah(1,2)'" do
    gen [:fcall, :blah, [:array, [:fixnum, 1], [:fixnum, 2]]] do |g|
      g.push :self
      g.push 1
      g.push 2
      g.send :blah, 2, true
    end
  end

  it "compiles 'blah(*a)'" do
    gen [:fcall, :blah, [:splat, [:vcall, :a]]] do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :blah, 0, true, false
    end
  end

  it "compiles 'block_given?'" do
    gen [:fcall, :block_given?] do |g|
      g.push_block
    end
  end

  it "compiles 'iterator?'" do
    gen [:fcall, :iterator?] do |g|
      g.push_block
    end
  end


  it "compiles 'self.blah(1)'" do
    x = [:call, [:self], :blah, [:array, [:fixnum, 1]]]

    gen x do |g|
      g.push :self
      g.push 1
      g.send :blah, 1, false
    end
  end

  it "compiles '1.go(2)'" do
    x = [:call, [:fixnum, 1], :go, [:array, [:fixnum, 2]]]

    gen x do |g|
      g.push 1
      g.push 2
      g.send :go, 1, false
    end
  end

  it "compiles '10.times(2) { 12 }'" do
    x = [:iter,
          [:call, [:lit, 10], :times, [:array, [:fixnum, 2]]],
          nil,
          [:lit, 12]
        ]

    gen x do |g|
      desc = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.pop_modifiers
        d.ret
      end

      g.push 10
      g.push 2
      g.push_literal desc
      g.create_block
      g.passed_block do
        g.send_with_block :times, 1, false
      end
    end
  end

  it "compiles 'a.b(*c) { 12 }'" do
    x = [:iter,
          [:call, [:vcall, :a], :b, [:splat, [:vcall, :c]]],
          nil,
          [:lit, 12]
        ]

    gen x do |g|
       desc = description do |d|
         d.pop
         d.push_modifiers
         d.new_label.set! # redo
         d.push 12
         d.pop_modifiers
         d.ret
       end
       g.push :self
       g.send :a, 0, true

       g.push :self
       g.send :c, 0, true
       g.cast_array
       g.push_literal desc
       g.create_block

       g.passed_block do
         g.send_with_splat :b, 0, false, false
       end
    end
  end

  it "compiles 'h[:blah] = 8'" do
    x = [:attrasgn, [:vcall, :h], :[]=, [:array, [:lit, :blah], [:fixnum, 8]]]

    gen x do |g|
      g.push :self
      g.send :h, 0, true
      g.push_unique_literal :blah
      g.push 8
      g.send :[]=, 2, false
    end
  end

  it "compiles 'a.b = 8'" do
    x = [:attrasgn, [:vcall, :a], :b, [:array, [:fixnum, 8]]]

    gen x do |g|
      g.push :self
      g.send :a, 0, true
      g.push 8
      g.send :b=, 1, false
    end
  end

  it "compiles 'self[index, 0] = other_string'" do
    x = [:attrasgn, nil, :[]=,
      [:array, [:vcall, :index], [:fixnum, 0],
        [:vcall, :other_string]
      ]
    ]

    gen x do |g|
      g.push :self
      g.push :self
      g.send :index, 0, true
      g.push 0
      g.push :self
      g.send :other_string, 0, true
      g.send :[]=, 3, false
    end
  end

  # Dynamic argument count specs
  it "compiles 'h(1, 2, *a)'" do
    x = [:fcall, :h, [:argscat, [:array, [:fixnum, 1], [:fixnum, 2]],
            [:vcall, :a]]]
    gen x do |g|
      g.push :self
      g.push 1
      g.push 2

      g.push :self
      g.send :a, 0, true
      g.cast_array

      g.push :nil
      g.send_with_splat :h, 2, true, false
    end
  end

  it "compiles 'f[*x] = 3'" do
    x = [:attrasgn, [:vcall, :f], :[]=,
        [:argspush, [:splat, [:vcall, :x]], [:fixnum, 3]]]

    gen x do |g|
      g.push :self
      g.send :f, 0, true
      g.push :self
      g.send :x, 0, true
      g.cast_array
      g.push 3
      g.swap
      g.push :nil
      g.send_with_splat :[]=, 1, false, true
    end
  end

  it "compiles 'undef :blah'" do
    gen [:undef, :blah] do |g|
      g.push :self
      g.send :metaclass, 0
      g.push_literal :blah
      g.send :undef_method, 1
    end
  end

  it "compiles 'class B; undef :blah; end'" do
    x = [:class, [:colon2, :A], nil, [:scope, [:undef, :blah], []]]

    gen x do |g|
      desc = description do |d|
        d.push :self
        d.push_literal :blah
        d.send :undef_method, 1
        d.ret
      end

      g.push :nil
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0

    end
  end

  it "compiles 'yield'" do
    x = [:yield, nil, false]

    gen x do |g|
      g.push_block
      g.meta_send_call 0
    end
  end

  it "compiles 'yield 1'" do
    x = [:yield, [:fixnum, 1], false]

    gen x do |g|
      g.push_block
      g.push 1
      g.meta_send_call 1
    end
  end

  it "compiles 'yield 1, 2'" do
    x = [:yield, [:array, [:fixnum, 1], [:fixnum, 2]], true]

    gen x do |g|
      g.push_block
      g.push 1
      g.push 2
      g.meta_send_call 2
    end
  end

  it "compiles 'yield [1, 2]'" do
    x = [:yield, [:array, [:fixnum, 1], [:fixnum, 2]], false]

    gen x do |g|
      g.push_block
      g.push 1
      g.push 2
      g.make_array 2
      g.meta_send_call 1
    end
  end

  it "compiles 'yield *a'" do
    x = [:yield, [:splat, [:vcall, :a]], false]

    gen x do |g|
      g.push_block
      g.push :self
      g.send :a, 0, true
      g.cast_array

      g.push :nil
      g.send_with_splat :call, 0, false, false
    end
  end

  it "compiles 'super(1,2)'" do
    x = [:defn, :a, [:scope, [:block, [:args],
          [:super, [:array, [:fixnum, 1], [:fixnum, 2]]]], []]]

    gen x do |g|
      desc = description do |d|
        d.push 1
        d.push 2
        d.push :nil
        d.send_super :a, 2
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'super(*blah)'" do
    x = [:defn, :a, [:scope, [:block, [:args],
          [:super, [:splat, [:vcall, :blah]]]], []]]

    gen x do |g|
      desc = description do |d|
        d.push :self
        d.send :blah, 0, true
        d.cast_array
        d.push :nil
        d.send_super_with_splat :a, 0
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'super'" do
    x = [:defn, :a, [:scope, [:block, [:args, [:a, :b], [], nil, nil],
          [:zsuper]], []]]

    gen x do |g|
      desc = description do |d|
        d.push_local 0
        d.push_local 1
        d.push :nil
        d.send_super :a, 2
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end

  end

  it "compiles 'super' with a splat in the arg list" do
    x = [:defn, :a, [:scope, [:block, [:args, [:name], [], [:rest, 1], nil],
          [:zsuper]], []]]

    gen x do |g|
      desc = description do |d|
        d.push_local 0
        d.push_local 1
        d.cast_array
        d.push :nil
        d.send_super_with_splat :a, 1
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end

  end

  it "compiles 'foo(&blah)'" do
    gen [:block_pass, [:vcall, :blah], [:fcall, :foo]] do |g|
      g.push :self
      g.push :self
      g.send :blah, 0, true
      g.dup
      g.is_nil
      lbl = g.new_label
      g.git lbl
      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1
      lbl.set!
      g.send_with_block :foo, 0, true
    end
  end

  it "compiles 'END { 666 }'" do
    gen [:iter, [:postexe], nil, [:lit, 666]] do |g|
      desc = description do |d|
        d.pop
        d.push_modifiers
        redo_label = d.new_label
        redo_label.set!
        d.push 666
        d.pop_modifiers
        d.ret
      end
      desc.name = :__block__
      desc.required = -1

      g.push :self
      g.push_literal desc
      g.create_block
      g.passed_block do
        g.send_with_block :at_exit, 0, true
      end
    end
  end
end
