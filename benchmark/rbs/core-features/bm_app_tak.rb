def tak(x, y, z)
  unless y < x
    z
  else
    tak(tak(x-1, y, z),
         tak(y-1, z, x),
         tak(z-1, x, y))
  end
end

Bench.run [5, 6, 7] do |n|
  tak 18, n, 0
end
