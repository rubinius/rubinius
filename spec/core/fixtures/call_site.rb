module CallSiteSpecs
  module M
    def call_sites(name)
      executable(name).call_sites
    end

    def constant_caches(name)
      executable(name).constant_caches
    end

    def executable(name)
      method_table.lookup(name).method
    end
  end

  module N
    def e
    end
  end

  class A
    extend M
    include N

    def a(b)
      b.c
    end

    def aa(b)
      b.c
    end

    def c
      d
    end

    def d
      e
    end

    def n
      e
    end

    def nn
      aa B.new
      aa BB.new
    end

    def p
      a B.new
      a BB.new
    end
  end

  class B
    extend M

    def c
    end
  end

  class BB < B
    def c
    end
  end

  class C
    extend M

    CC = 1

    def c
      CC
    end

    def cc
      C::CC
    end
  end

  class R
    extend M

    def c
      respond_to? :e
    end

    def d
      respond_to? :x
    end

    def dd
      respond_to? :xx
    end

    def e
    end

    def n
      respond_to? :e
    end
  end

  class RR
    extend M

    def a
      respond_to? :x
    end

    def b
      respond_to? :x, true
    end

    def respond_to_missing?(name, include_private)
      true
    end
  end
end
