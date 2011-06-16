module LangMethodSpecs
  module_function

  def fooP0; 100 end
  def fooP1(a); [a]; end
  def fooP2(a,b); [a,b]; end
  def fooP3(a,b,c); [a,b,c]; end
  def fooP4(a,b,c,d); [a,b,c,d]; end
  def fooP5(a,b,c,d,e); [a,b,c,d,e]; end
  def fooP0O1(a=1); [a]; end
  def fooP1O1(a,b=1); [a,b]; end
  def fooP2O1(a,b,c=1); [a,b,c]; end
  def fooP3O1(a,b,c,d=1); [a,b,c,d]; end
  def fooP4O1(a,b,c,d,e=1); [a,b,c,d,e]; end
  def fooP0O2(a=1,b=2); [a,b]; end
  def fooP0R(*r); r; end
  def fooP1R(a, *r); [a, r]; end
  def fooP0O1R(a=1, *r); [a, r]; end
  def fooP1O1R(a, b=1, *r); [a, b, r]; end


  def one(a); a; end
  def oneb(a,&b); [a,yield(b)]; end
  def makeproc(&b) b end

  # check that converted proc is indeed behaves like proc,
  # not like lambda
  def proc_caller(&b) b.call end
  def enclosing_method
    proc_caller { return :break_return_value }
    :method_return_value
  end

  def yield_now(x); yield x; end

  def double(x); x * 2 end
  def weird_parens
    # means double((5).to_s)
    # NOT   (double(5)).to_s
    double (5).to_s
  end

  def self.twos(a,b,*c)
    [c.size, c.last]
  end

  class PrivateSetter
    attr_reader :foo
    attr_writer :foo
    private :foo=

      def call_self_foo_equals(value)
        self.foo = value
      end

    def call_self_foo_equals_masgn(value)
      a, self.foo = 1, value
    end
  end
  
  class PrivateGetter
    attr_reader :foo
    private :foo

    def call_self_foo
      self.foo
    end

    def call_self_foo_or_equals(value)
      self.foo ||= 6
    end
  end
end
