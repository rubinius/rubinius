def fact(n)
  if(n > 1)
    n * fact(n-1)
  else
    1
  end
end

Bench.run [1000, 2000, 5000, 10000] do |n|
  fact n
end
