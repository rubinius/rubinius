require 'sexp/processor'

class CaseConverter < SexpProcessor
  def initialize
    super
    @support_switch = false
    self.expected = Array
    @unique_id = 1
  end
  
  attr_accessor :support_switch
  
  def uniq
    id = (@unique_id += 1)
    ["__uqcs#{id}".to_sym, id]
  end
  
  def as_call(var, cond)
    [:call, var, :===, [:array, cond]]
  end
  
  def detect_numbers_only(x)
    x[2].each do |whn|
      ary = whn[1].dup
      ary.shift
      ary.each do |elem|
        if elem.first != :lit
          return false
        elsif not elem.last.kind_of?(Fixnum)
          return false
        end
      end
    end
    
    return true
  end
  
  def into_switch(x)
    x.shift
    cond = x.shift
    whens = x.shift
    
    name, id = uniq()
    
    asgn = [:lasgn, name, id, cond]
    var  = [:lvar, name, id]
    
    whens.map! do |whn|
      whn.shift
      whn
    end
    
    [:switch, cond, whens]
  end
  
  def process_case(x)
    if @support_switch and detect_numbers_only(x)
      return into_switch(x)
    end
    
    x.shift
    cond = x.shift
    whens = x.shift
    
    name, id = uniq()
    
    asgn = [:lasgn, name, id, cond]
    var  = [:lvar, name, id]
    
    ncond = []
    
    cur = nil
    whens.reverse.each do |wc|
      wc.shift
      ary = wc.shift
      body = wc.shift
      ary.shift
      if ary.size == 1
        this_cond = as_call var, ary.shift
      else
        ary.reverse!
        this_cond = [:or, as_call(var, ary.shift), as_call(var, ary.shift)]
        this_cond = ary.inject(this_cond) do |acc, elem|
          [:or, as_call(var, elem), acc]
        end
      end
      
      me = [:if, this_cond, body]
      me << cur if cur
      cur = me
    end
    
    [:block, asgn, cur]
  end
end