def fact(n)
  if(n > 1)
    n * fact(n-1)
  else
    1
  end
end

20.times do
  fact(4000)
  fact(4000)
  fact(4000)
  fact(4000)
  fact(4000)
end

