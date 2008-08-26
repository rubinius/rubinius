require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles simple lasgn / lvar" do
    ruby = <<-EOC
      a = 12
      a
    EOC

    sexp = s(:block,
             s(:lasgn, :a, s(:fixnum, 12)),
             s(:lvar, :a, 0))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end

  it "compiles lvars only after declared" do
    ruby = <<-EOC
      name
      name = 3
      name
    EOC

    sexp = s(:block, s(:vcall, :name),
             s(:lasgn, :name, s(:lit, 3)),
             s(:lvar, :name, 0))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push :self
      g.send :name, 0, true
      g.pop
      g.push 3
      g.set_local 0
      g.pop
      g.push_local 0
    end

  end

  it "compiles lvar for argument" do # def meth(b); b; end
    ruby = <<-EOC
      def meth(b)
        b
      end
    EOC

    sexp = s(:defn, :meth,
             s(:scope,
               s(:block,
                 s(:args, s(:b), s(), nil, nil),
                 s(:lvar, :b, 0)), s()))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      meth = description do |d|
        d.push_local 0
        d.ret
      end

      g.push :self
      g.push_literal :meth
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles toplevel lvar into locals when closure referenced" do
    ruby = <<-EOC
      name = 12
      go do
        name
      end
    EOC

    sexp = s(:block,
             s(:lasgn, :name, s(:fixnum, 12)),
             s(:iter, s(:fcall, :go), nil, s(:lvar, :name, 0)))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push_local 0
        d.pop_modifiers
        d.ret
      end

      g.push 12
      g.set_local 0
      g.pop
      g.push :self
      g.create_block iter
      g.passed_block(1) do
        g.send_with_block :go, 0, true
      end
    end
  end

  it "compiles block lvar into depth referenced lvar" do
    ruby = <<-EOC
      go do
        name = 12
        name
      end
    EOC

    sexp = s(:iter, s(:fcall, :go), nil,
             s(:block,
               s(:lasgn, :name, s(:fixnum, 12)),
               s(:lvar, :name, 0)))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop
        d.push_local_depth 0, 0
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.create_block iter
      g.passed_block do
        g.send_with_block :go, 0, true
      end
    end
  end

  it "compiles block lvar into depth referenced lvar upward" do
    ruby = <<-EOC
      go do
        name = 12
        go do
          name
        end
      end
    EOC

    sexp = s(:iter,
             s(:fcall, :go),
             nil,
             s(:block,
               s(:lasgn, :name, s(:fixnum, 12)),
               s(:iter,
                 s(:fcall, :go),
                 nil,
                 s(:block, s(:lvar, :name, 0)))))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop

        i2 = description do |j|
          j.pop
          j.push_modifiers
          j.new_label.set! # redo
          j.push_local_depth 1, 0
          j.pop_modifiers
          j.ret
        end

        d.push :self
        d.create_block i2
        d.passed_block(0, true) do
          d.send_with_block :go, 0, true
        end
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.create_block iter
      g.passed_block do
        g.send_with_block :go, 0, true
      end
    end
  end

  # Ivars and slots
  it "compiles '@blah'" do
    ruby = <<-EOC
      @blah
    EOC

    sexp = s(:ivar, :@blah)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_ivar :@blah
    end
  end

  it "compiles '@blah = 1'" do
    ruby = <<-EOC
      @blah = 1
    EOC

    sexp = s(:iasgn, :@blah, s(:lit, 1))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 1
      g.set_ivar :@blah
    end
  end

  it "compiles '$var'" do
    ruby = <<-EOC
      $var
    EOC

    sexp = s(:gvar, :$var)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_cpath_top
      g.find_const :Globals
      g.push_literal :$var
      g.send :[], 1
    end
  end

  it "compiles '$var = 1'" do
    ruby = <<-EOC
      $var = 1
    EOC

    sexp = s(:gasgn, :$var, s(:fixnum, 12))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push_cpath_top
      g.find_const :Globals
      g.push_literal :$var
      g.push 12
      g.send :[]=, 2
    end
  end

  it "compiles '$!'" do
    ruby = <<-EOC
      $!
    EOC

    sexp = s(:gvar, :$!)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_exception
    end
  end

  it "compiles '$! = 1'" do
    ruby = <<-EOC
      $! = 1
    EOC

    sexp = s(:gasgn, :$!, s(:fixnum, 1))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push 1
      g.raise_exc
    end
  end

  it "compiles 'String'" do
    ruby = <<-EOC
      String
    EOC

    sexp = s(:const, :String)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_const :String
    end
  end

  it "compiles 'Object::String'" do
    ruby = <<-EOC
      Object::String
    EOC

    sexp = s(:colon2, s(:const, :Object), :String)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_const :Object
      g.find_const :String
    end
  end

  it "compiles '::String'" do
    ruby = <<-EOC
      ::String
    EOC

    sexp = s(:colon3, :String)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_cpath_top
      g.find_const :String
    end
  end

  it "compiles 'Blah = 1'" do
    ruby = <<-EOC
      Blah = 1
    EOC

    sexp = s(:cdecl, :Blah, s(:lit, 1), nil)

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push :self
      g.push_literal :Blah
      g.push 1
      g.send :__const_set__, 2
    end
  end

  it "compiles 'Object::Blah = 1'" do
    ruby = <<-EOC
      Object::Blah = 1
    EOC

    sexp = s(:cdecl, nil, s(:lit, 1), s(:colon2, s(:const, :Object), :Blah))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push_const :Object
      g.push_literal :Blah
      g.push 1
      g.send :__const_set__, 2
    end
  end

  it "compiles '::Blah = 1'" do
    ruby = <<-EOC
      ::Blah = 1
    EOC

    sexp = s(:cdecl, nil, s(:lit, 1), s(:colon3, :Blah))

    sexp.should == parse(ruby) if $unified

    gen sexp do |g|
      g.push_cpath_top
      g.push_literal :Blah
      g.push 1
      g.send :__const_set__, 2
    end
  end
end
