module RespondToCacheSpec

  class Bar
    CallSiteTrue = instance_method(def call_site_true
      respond_to?(:hits_test)
    end).executable

    CallSiteFalse = instance_method(def call_site_false
      respond_to?(:hits_test_non_existing)
    end).executable

    HitTest = instance_method(def hits_test
      respond_to?(:hits_test)
    end).executable

    CacheTest = instance_method(def call_site_respond_to_cache
      respond_to?(:call_site_respond_to_cache_method)
    end).executable
  end

  class BarMissing
    CallSiteTrue = instance_method(def call_site_true
      respond_to?(:hits_test)
    end).executable

    def respond_to_missing?(name, include_private)
      true
    end
  end
end

