require File.dirname(__FILE__) + '/../spec_helper'

describe "A Op_asgn1 node" do
  relates <<-ruby do
      @b = []
      @b[1] ||= 10
      @b[2] &&= 11
      @b[3] += 12
    ruby

    parse do
      [:block,
       [:iasgn, :@b, [:array]],
       [:op_asgn1, [:ivar, :@b], [:arglist, [:lit, 1]], :"||", [:lit, 10]],
       [:op_asgn1, [:ivar, :@b], [:arglist, [:lit, 2]], :"&&", [:lit, 11]],
       [:op_asgn1, [:ivar, :@b], [:arglist, [:lit, 3]], :+, [:lit, 12]]]
    end

    # op asgn1 ivar
  end

  relates <<-ruby do
      b = []
      b[1] ||= 10
      b[2] &&= 11
      b[3] += 12
    ruby

    parse do
      [:block,
       [:lasgn, :b, [:array]],
       [:op_asgn1, [:lvar, :b], [:arglist, [:lit, 1]], :"||", [:lit, 10]],
       [:op_asgn1, [:lvar, :b], [:arglist, [:lit, 2]], :"&&", [:lit, 11]],
       [:op_asgn1, [:lvar, :b], [:arglist, [:lit, 3]], :+, [:lit, 12]]]
    end

    # op asgn1
  end

end

describe "A Op_asgn2 node" do
  relates "self.Bag ||= Bag.new" do
    parse do
      [:op_asgn2,
        [:self],
        :"Bag=", :"||", [:call, [:const, :Bag], :new, [:arglist]]]
    end

    # op asgn2 self
  end

  relates <<-ruby do
      s = Struct.new(:var)
      c = s.new(nil)
      c.var ||= 20
      c.var &&= 21
      c.var += 22
      c.d.e.f ||= 42
    ruby

    parse do
      [:block,
       [:lasgn, :s, [:call, [:const, :Struct], :new, [:arglist, [:lit, :var]]]],
       [:lasgn, :c, [:call, [:lvar, :s], :new, [:arglist, [:nil]]]],
       [:op_asgn2, [:lvar, :c], :var=, :"||", [:lit, 20]],
       [:op_asgn2, [:lvar, :c], :var=, :"&&", [:lit, 21]],
       [:op_asgn2, [:lvar, :c], :var=, :+, [:lit, 22]],
       [:op_asgn2,
        [:call, [:call, [:lvar, :c], :d, [:arglist]], :e, [:arglist]],
        :f=,
        :"||",
        [:lit, 42]]]
    end

    # op asgn2
  end
end

describe "A Op_asgn_and node" do
  relates "@fetcher &&= new(Gem.configuration[:http_proxy])" do
    parse do
      [:op_asgn_and,
       [:ivar, :@fetcher],
       [:iasgn,
        :@fetcher,
        [:call,
         nil,
         :new,
         [:arglist,
          [:call,
           [:call, [:const, :Gem], :configuration, [:arglist]],
           :[],
           [:arglist, [:lit, :http_proxy]]]]]]]
    end

    # op asgn and ivar2
  end

  relates <<-ruby do
      a = 0
      a &&= 2
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:lit, 0]],
       [:op_asgn_and, [:lvar, :a], [:lasgn, :a, [:lit, 2]]]]
    end

    # op asgn and
  end

end

describe "A Op_asgn_or node" do
  relates <<-ruby do
      a ||= begin
              b
            rescue
              c
            end
    ruby

    parse do
      [:op_asgn_or,
       [:lvar, :a],
       [:lasgn,
        :a,
        [:rescue,
         [:call, nil, :b, [:arglist]],
         [:resbody, [:array], [:call, nil, :c, [:arglist]]]]]]
    end

    # op asgn or block
  end

  relates "@fetcher ||= new(Gem.configuration[:http_proxy])" do
    parse do
      [:op_asgn_or,
       [:ivar, :@fetcher],
       [:iasgn,
        :@fetcher,
        [:call,
         nil,
         :new,
         [:arglist,
          [:call,
           [:call, [:const, :Gem], :configuration, [:arglist]],
           :[],
           [:arglist, [:lit, :http_proxy]]]]]]]
    end

    # op asgn or ivar2
  end

  relates "@v ||= {  }" do
    parse do
      [:op_asgn_or, [:ivar, :@v], [:iasgn, :@v, [:hash]]]
    end

    # op asgn or ivar
  end

  relates <<-ruby do
      a = 0
      a ||= 1
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:lit, 0]],
       [:op_asgn_or, [:lvar, :a], [:lasgn, :a, [:lit, 1]]]]
    end

    # op asgn or
  end
end
