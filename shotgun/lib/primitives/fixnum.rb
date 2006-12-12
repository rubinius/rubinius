class FixnumPrimitives
  def add(t1)
    fail unless t1.fixnum?
    j = self.to_int
    k = t1.to_int
    m = j + k
    t2 = m.to_fix
    if(m != t2.to_int)
      t2 = bignum_add(state, bignum_new(state, j), bignum_new(state, k))
    end
    return t2
  end
  
  def sub(t1)
    fail unless t1.fixnum?
    j = self.to_int
    k = t1.to_int
    m = j - k
    t2 = m.to_fix
    if(m != t2.to_int)
      t2 = bignum_sub(state, bignum_new(state, j), bignum_new(state, k))
    end
    return t2
  end
  
  def fixnum_mul(t1)
    fail unless t1.fixnum?
    j = self.to_int
    k = t1.to_int
    m = j * k
    t2 = m.to_fix
    if(m != t2.to_int)
      t2 = bignum_mul(state, bignum_new(state, j), bignum_new(state, k))
    end
    return t2
  end
  
  def equal(t1)
    fail unless t1.fixnum?
    if self == t1
      return true
    else
      return false
    end
  end
  
  def compare(t1)
    fail unless t1.fixnum?
    
    return 0.to_fix if self == t1
    j = self.to_int
    k = t1.to_int
    if j < k
      return -1.to_fix
    else
      return 1.to_fix
    end
  end
  
  def fixnum_to_s(t1)
    if t1.fixnum?
      buf.declare "char*", "char buf[100]"
      b.declare "char*", "char *b"
      digitmap.declare "char*"
      
      b = buf + sizeof(buf)
      
      j = t1.to_int
      k = self.to_int
      fail if j < 2 or 36 < j
      
      if k == 0
        return string_new(state, "0")
      end
      
      m = 0
      if k < 0
        k = 0 - k
        m = 1
      end
      b.set_value "\0".as_char
      b -= 1
      
      b.set_value digitmap[k % j];
      b -= 1
      k = k / j
      while k
        b.set_value digitmap[k % j];
        b -= 1
        k = k / j
      end
      if m
        b.set_value "-".as_char
        b -= 1
      end
      
      return string_new(state, b)
    else
      fail
    end
  end
end