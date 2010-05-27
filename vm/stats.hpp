namespace rubinius {
  struct Stats {
    size_t jitted_methods;
    size_t jit_time_spent;

    Stats()
      : jitted_methods(0)
      , jit_time_spent(0)
    {}
  };
}
