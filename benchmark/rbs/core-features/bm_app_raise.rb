def raise_and_rescue(n)
  n.times do
    begin
      raise
    rescue
    end
  end
end

Bench.run [300_000] do |n|
  raise_and_rescue n
end
