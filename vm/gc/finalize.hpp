namespace rubinius {

  typedef void (*FinalizerFunction)(STATE, Object*);

  struct FinalizeObject {
  public:
    enum FinalizationStatus {
      eLive,
      eQueued,
      eFinalized
    };

  public:
    FinalizeObject()
      : queue_count(0)
      , finalizer(0)
    {}

    Object* object;
    FinalizationStatus status;
    int queue_count;
    FinalizerFunction finalizer;

    void queued() {
      status = eQueued;
    }
  };
}
