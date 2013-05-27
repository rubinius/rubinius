module RespondToCacheSpec

  class Bar
    CallSiteTrue = def call_site_true
      respond_to?(:hits_test)
    end

    CallSiteFalse = def call_site_false
      respond_to?(:hits_test_non_existing)
    end

    HitTest = def hits_test
      respond_to?(:hits_test)
    end

    CacheTest = def call_site_respond_to_cache
      respond_to?(:call_site_respond_to_cache_method)
    end
  end
end

