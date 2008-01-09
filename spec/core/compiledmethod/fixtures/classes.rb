module CompiledMethodSpecs

  def simple_puts
    a = 'hello'
    puts a
  end

  def mthd_with_splat(a,b,*c)
    return a+b > c.size
  end

  def more_complex
    mthd_with_splat 1,*[2,3,4,5]
  end

end
