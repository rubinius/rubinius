module Signal
  Rubinius::RUBY_CONFIG.keys.each do |key|
    if key[0, 20] == 'rbx.platform.signal.'
      Names[ key[23, 100] ] = Rubinius::RUBY_CONFIG[key]
    end
  end

  # special case of signal.c
  if Names["CHLD"]
    Names["CLD"] = Names["CHLD"]
  end
end
